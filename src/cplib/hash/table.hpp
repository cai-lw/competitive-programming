#include <cassert>
#include <iterator>
#include <variant>
#include "cplib/hash/wyhash.hpp"

namespace cplib {

namespace impl {

template<typename T>
struct HashCell {
    enum State {
        EMPTY,
        PHANTOM,
        SENTINEL
    };

    std::variant<State, T> inner;

    HashCell() : inner(State::EMPTY) {}

    bool empty() const { return inner.index() == 0 && std::get<0>(inner) == State::EMPTY; }

    bool occupied() const { return inner.index() == 1; }

    bool phantom() const { return inner.index() == 0 && std::get<0>(inner) == State::PHANTOM; }

    bool sentinel() const { return inner.index() == 0 && std::get<0>(inner) == State::SENTINEL; }

    void insert(const T& val) { inner = val; }

    void insert(T&& val) { inner = val; }

    const T& value() const { return std::get<1>(inner); }

    T& value() { return std::get<1>(inner); }

    void erase() { inner = State::PHANTOM; }
};

}  // namespace impl

/**
 * \brief Linear probing hash table with `std` non-compliant interface.
 * 
 * This hash table aims to be fast for typical competitive programming use cases.
 * Insertion, lookup and deletion takes expected \f$O(1)\f$ time.
 * 
 * Only the lower bits of the hash value are used, so the hash function must be reasonably chaotic, and
 * for integer types `std::hash` is not acceptable. By default it uses WyHash.
 * 
 * Due to lazy deletion, there are two load factors, the larger "non-empty" load factor which counts cells marked
 * for deletion, and the smaller "occupied" load factor which doesn't.
 * The rehashing policy is that both load factors are always within \f$[\frac{1}{8},\frac{1}{2}]\f$, unless the
 * capacity is manually set (see HashTable::reserve). The lower is required for \f$O(N)\f$ traversal.
 * Additinoally, the capacity is always a power of two, and is at least 4.
 * 
 * \tparam T Type of elements.
 * \tparam Hash Hash function object.
 * \tparam Eq Equality comparison function object.
 */
template<typename T, typename Hash = WyHash<T>, typename Eq = std::equal_to<T>>
class HashTable {
public:
    using size_type = std::size_t;
    using value_type = T;
    using iterator_type = impl::HashCell<T>*;

    /** \brief Constructs an empty hash table.
     * 
     * Note that this constructor allocates cells for a capacity of 4 even for an empty hash table.
     */
    HashTable() : nonempty(0), occupied(0), disable_shrink(false) { _allocate_cells(4); }

    /** \brief Constructs a hash table containing elements from a pair of iterators.
     * 
     * If the iterators are random access iterators, the number of elements is known, so sufficient space will be
     * allocated in advance, so that no rehashing happens during construction.
     */
    template<typename InputIt>
    HashTable(InputIt first, InputIt last) : nonempty(0), occupied(0), disable_shrink(false) {
        size_type cap = 4;
        if (std::is_same_v<std::iterator_traits<InputIt>::iterator_category, std::random_access_iterator_tag>) {
            auto dist = std::distance(first, last);
            while (dist > (cap >> 1)) {
                cap <<= 1;
            }
        }
        _allocate_cells(cap);
        for (; first != last; ++first) {
            insert(*first);
        }
    }

    ~HashTable() { delete[] cells; }

    /** \brief Returns the number of elements in the hash table. */
    size_type size() const { return occupied; }

    /** \brief Returns whether the hash table contains no element. */
    bool empty() const { return occupied == 0; }

    /** \brief Returns the size of the underlying array of cells.
     * 
     * This is always a power of two.
     */
    size_type capacity() const { return cap_mask + 1; }

    /** \brief Returns pointer to the beginning of the cell array. */
    impl::HashCell<T>* cell_begin() const { return cells; }

    impl::HashCell<T>* cell_end() const { return cells + capacity(); }

    /**
     * \brief Find an element that compares equal to the given value, or an empty cell if not found.
     * 
     * \tparam Revive If `true`, may instead return a "phantom" (marked for deletion) cell when the value is not found.
     * Used by HashTable::insert to reuse "phantom" cells and reduce rehashing.
     */
    template<bool Revive = false>
    impl::HashCell<T>* find_cell(const T &x) const {
        size_type loc = hash(x) & cap_mask;
        impl::HashCell<T>* phantom = nullptr;
        while (!cells[loc].empty()) {
            if (cells[loc].occupied() && eq(x, cells[loc].value())) {
                return cells + loc;
            }
            if (Revive && cells[loc].phantom()) {
                phantom = cells + loc;
            }
            loc = (loc + 1) & cap_mask;
        }
        if (Revive && phantom) {
            return phantom;
        } else {
            return cells + loc;
        }
    }

    /** \brief Returns whether an element that compares equal to the given value is found in the hash table. */
    bool contains(const T &x) const { return !find_cell(x)->empty(); }

    /**
     * \brief Insert an element if there is no element already present that compares equal to it.
     * 
     * \tparam Replace When an element that compares equal to `x` is found, will replace it if `true`,
     * and will do nothing if `false`.
     */
    template<bool Replace = false>
    bool insert(const T &x) {
        impl::HashCell<T> *cell = find_cell<true>(x);
        bool do_insert = true;
        if (!cell->occupied()) {
            ++occupied;
            if (cell->empty()) {
                ++nonempty;
            }
            cell->insert(x);
            _check_rehash();
        } else if (Replace) {
            cell->insert(x);
        } else {
            do_insert = false;
        }
        return do_insert;
    }

    /** \copydoc insert(const T&) */
    template<bool Replace = false>
    bool insert(T&& x) {
        impl::HashCell<T> *cell = find_cell<true>(x);
        bool do_insert = true;
        if (!cell->occupied()) {
            ++occupied;
            if (cell->empty()) {
                ++nonempty;
            }
            cell->insert(std::move(x));
            _check_rehash();
        } else if (Replace) {
            cell->insert(std::move(x));
        } else {
            do_insert = false;
        }
        return do_insert;
    }

    /** \brief Remove the element that compares equal to the given value if it is present. */
    bool erase(const T &x) {
        impl::HashCell<T> *cell = find_cell(x);
        bool do_erase = !cell->empty();
        if (do_erase) {
            cell->erase();
            --occupied;
            _check_rehash();
        }
        return do_erase;
    }

    /**
     * \brief Allocates the given capacity, and rehashes all elements.
     * 
     * \param new_cap The new capacity. Must be a power of two and at least 4.
     */
    void rehash(size_type new_cap) {
        assert(new_cap >= 4 && (new_cap & (new_cap - 1)) == 0);
        impl::HashCell<T>* old_cells = cells;
        size_type old_cap = cap_mask + 1;
        _allocate_cells(new_cap);
        nonempty = occupied;
        for (size_type i = 0; i < old_cap; ++i) {
            if (old_cells[i].occupied()) {
                size_type loc = hash(old_cells[i].value()) & cap_mask;
                while (!cells[loc].empty()) {
                    loc = (loc + 1) & cap_mask;
                }
                cells[loc] = std::move(old_cells[i]);
            }
        }
        delete[] old_cells;
    }

    /**
     * \brief Reserve capacity and temporary disable shrinking.
     * 
     * If current capacity can hold at least `new_size` non-empty cells, does nothing. Note that if there are already
     * some deleted cells, which are also considered non-empty, the hash table may not be able to hold `new_size`
     * elements without rehashing.
     * 
     * Otherwise, rehashes and allocates enough capacity for holding at least `new_size` non-empty cells, After this,
     * the capacity will never "naturally" shrink (due to low load factor), unless it has "naturally" grown
     * (due to high load factor) at least once.
     * 
     * Note that the cost of traversing the hash table is proportional to its capacity, not its size. Thus traversal
     * should be avoided after calling this function until all desired elements are inserted.
     */
    void reserve(size_type new_size) {
        size_type new_cap = capacity();
        while (new_size > (new_cap >> 1)) {
            new_cap <<= 1;
        }
        if (new_cap > capacity()) {
            rehash(new_cap);
            disable_shrink = true;
        }
    }

private:
    impl::HashCell<T>* cells;
    size_type nonempty, occupied, cap_mask;
    bool disable_shrink;
    Hash hash;
    Eq eq;

    // allocate a new array of cells. Existing cells are NOT deleted.
    void _allocate_cells(size_type cap) {
        cells = new impl::HashCell<T>[cap + 1];
        cells[cap].inner = impl::HashCell<T>::State::SENTINEL;
        cap_mask = cap - 1;
    }

    // See class documentation for rehashing policy.
    void _check_rehash() {
        size_type cap = capacity();
        if (!disable_shrink && occupied < (cap >> 3)) {
            do {
                cap >>= 1;
            } while (occupied < (cap >> 3));
            rehash(cap);
        } else if (nonempty > (cap >> 1)) {
            disable_shrink = false;
            if (occupied > (cap >> 2)) {
                cap <<= 1;
            }
            rehash(cap);
        }
    }
};

}  // namespace cplib