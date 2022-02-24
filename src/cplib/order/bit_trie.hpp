#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <utility>
#include <variant>
#include <vector>
#include "cplib/utils.hpp"

namespace cplib {

/**
 * \brief Bit trie that manages an ordered set of integers, with compile-time fixed height.
 * 
 * Most operations take \f$O(U/B)\f$ time, where \f$U\f$ is the number of bit in each element, and \f$B\f$ is
 * the number of bits per level in the tree. On 64-bit machines \f$B=\log_2 64=6\f$.
 * 
 * The implementation mostly follows https://en.wikipedia.org/wiki/Bitwise_trie_with_bitmap.
 * 
 * Your code should use the type alias ::BitTrie instead of this class.
 * 
 * \tparam T The type of the elements. Should be an unsigned integer type.
 * \tparam Bits Number of bits to look at per level.
 * \tparam Offset The starting position of bits to look at for the current level.
 */
template<typename T, int Bits, int Offset>
class StaticSizedBitTrie {
public:
    using Child = StaticSizedBitTrie<T, Bits, Offset - Bits>;
    using bitmap_t = uint64_t;
    using size_type = std::size_t;
    static constexpr int branch_mask = (1 << Bits) - 1;

    /** \brief Creates an empty trie. */
    StaticSizedBitTrie() : size_(0), bitmap(0) {}

    /** \brief Returns the number of elements. */
    size_type size() const { return size_; }

    /** \brief Returns whether the trie is empty. */
    bool empty() const { return size_ == 0; }

    /** \brief Insert an elemnent. Returns whether the element is inserted (i.e. doesn't exist before insertion). */
    bool insert(T val) {
        int idx = (val >> Offset) & branch_mask;
        size_type pos = impl::popcount_low(bitmap, idx);
        if (!((bitmap >> idx) & 1)) {
            bitmap |= bitmap_t(1) << idx;
            children.emplace(children.begin() + pos);
        }
        bool ret = children[pos].insert(val);
        size_ += ret;
        return ret;
    }

    /** \brief Delete an elemnent. Returns whether the element is deleted (i.e. exists before deletion). */
    bool erase(T val) {
        int idx = (val >> Offset) & branch_mask;
        if ((bitmap >> idx) & 1) {
            size_type pos = impl::popcount_low(bitmap, idx);
            bool ret = children[pos].erase(val);
            size_ -= ret;
            if (children[pos].empty()) {
                bitmap &= ~(bitmap_t(1) << idx);
                children.erase(children.begin() + pos);
            }
            return ret;
        } else {
            return false;
        }
    }

    /** \brief Returns whether the element exists. */
    bool find(T val) const {
        int idx = (val >> Offset) & branch_mask;
        if ((bitmap >> idx) & 1) {
            size_type pos = impl::popcount_low(bitmap, idx);
            return children[pos].find(val);
        } else {
            return false;
        }
    }

    /**
     * \brief Returns the minimum of any element xor the give value.
     * 
     * In other words, this returns \f$\min_{a\in S}{a \oplus x}\f$, the minimum value after xor. To get the element
     * that minimizes the xor value (\f$\mathrm{argmin}\f$ instead of \f$\min\f$), simply xor the returned value with
     * the argument, since \f$a \oplus x \oplus x=a\f$
     */
    T xor_min(T xor_val) const {
        int curr_xor_val = (xor_val >> Offset) & branch_mask;
        int curr_xor_min = port::countr_zero(impl::xor_permute(bitmap, curr_xor_val));
        size_type pos = impl::popcount_low(bitmap, curr_xor_min ^ curr_xor_val);
        return (T(curr_xor_min) << Offset) | children[pos].xor_min(xor_val);
    }

    /** \brief Returns the smallest element no smaller than the given value, or `std::nullopt` if it doesn't exist. */
    std::optional<T> next(T val) const {
        int idx = (val >> Offset) & branch_mask;
        int next_idx = impl::next_set_bit(bitmap, idx);
        if (next_idx == 64) {
            return std::nullopt;
        } else {
            size_type pos = impl::popcount_low(bitmap, next_idx);
            if (next_idx > idx) {
                return (T(next_idx) << Offset) | *(children[pos].next(0));
            }
            auto ret = children[pos].next(val);
            if (ret) {
                return (T(next_idx) << Offset) | *ret;
            }
            next_idx = impl::next_set_bit(bitmap, next_idx + 1);
            if (next_idx == 64) {
                return std::nullopt;
            } else {
                return (T(next_idx) << Offset) | *(children[pos + 1].next(0));
            }
        }
    }

    /** \brief Returns the largest element no larger than the given value, or `std::nullopt` if it doesn't exist, */
    std::optional<T> prev(T val) const {
        int idx = (val >> Offset) & branch_mask;
        int prev_idx = impl::prev_set_bit(bitmap, idx + 1);
        if (prev_idx == -1) {
            return std::nullopt;
        } else {
            size_type pos = impl::popcount_low(bitmap, prev_idx);
            constexpr T MAX = (T(1) << Offset) - 1;
            if (prev_idx < idx) {
                return (T(prev_idx) << Offset) | *(children[pos].prev(MAX));
            }
            auto ret = children[pos].prev(val);
            if (ret) {
                return (T(prev_idx) << Offset) | *ret;
            }
            prev_idx = impl::prev_set_bit(bitmap, prev_idx);
            if (prev_idx == -1) {
                return std::nullopt;
            } else {
                return (T(prev_idx) << Offset) | *(children[pos - 1].prev(MAX));
            }
        }
    }

private:
    bitmap_t bitmap;
    size_type size_;
    std::vector<Child> children;
};

/** \brief Specialization of StaticSizedBitTrie on the lowest level, using only a bitmap. */
template<typename T, int Bits>
class StaticSizedBitTrie<T, Bits, 0> {
public:
    using bitmap_t = uint64_t;
    using size_type = std::size_t;
    static constexpr int branch_mask = (1 << Bits) - 1;

    StaticSizedBitTrie() : bitmap(0) {}

    size_type size() const { return port::popcount(bitmap); }

    bool empty() const { return bitmap == 0; }

    bool insert(T val) {
        int idx = val & branch_mask;
        bool ret = !((bitmap >> idx) & 1);
        bitmap |= bitmap_t(1) << idx;
        return ret;
    }

    bool erase(T val) {
        int idx = val & branch_mask;
        bool ret = (bitmap >> idx) & 1;
        bitmap &= ~(bitmap_t(1) << idx);
        return ret;
    }

    bool find(T val) const {
        return (bitmap >> (val & branch_mask)) & 1;
    }

    T xor_min(T xor_val) const {
        return port::countr_zero(impl::xor_permute(bitmap, xor_val & branch_mask));
    }

    std::optional<T> next(T val) const {
        int ret = impl::next_set_bit(bitmap, val & branch_mask);
        return ret == 64 ? std::nullopt : std::make_optional(ret);
    }

    std::optional<T> prev(T val) const {
        int ret = impl::prev_set_bit(bitmap, (val & branch_mask) + 1);
        return ret == -1 ? std::nullopt : std::make_optional(ret);
    }

private:
    bitmap_t bitmap;
};

/**
 * \related StaticSizedBitTrie
 * 
 * Convenient type alias for using StaticSizedBitTrie.
 * 
 * \tparam T The type of the elements. Should be an unsigned integer type.
 * \tparam U Number of bits in each element. That is, all elements are integers in the range of \f$[0,2^U-1]\f$.
 */
template<typename T, int U>
using BitTrie = StaticSizedBitTrie<T, 6, (U - 1) / 6 * 6>;

}  // namespace cplib
