#pragma once

#include <functional>
#include <vector>

namespace cplib {

/**
 * \brief D-ary heap, slightly faster than binary heap due to less random memory access.
 * \ingroup order
 * 
 * A drop-in replacement for `std::priority_queue`, but is min-heap by default, since min-heap is much more common.
 * 
 * For performance analysis, see https://en.wikipedia.org/wiki/D-ary_heap. 
 * 
 * \tparam T Type of elements.
 * \tparam Comp Comparison function object. Note that this is a min-heap, as opposed to `std::priority_queue`, so the 
 * element on the top is the smallest element ordered by the comparison function.
 * \tparam D Number of tree branches. It should be a power of two for fast arithmetics. The default value of 8 is
 * good for general competitive programming use. A larger D leads to faster insertion at the cost of slower deletion.
 */
template<typename T, typename Comp = std::less<T>, int D = 8>
struct DaryHeap {
public:
    using size_type = std::size_t;

    /** \brief Returns the number of elements. */
    size_type size() const { return arr.size(); }

    /** \brief Returns whether the heap is empty. */
    bool empty() const { return arr.empty(); }

    /** \brief Returns the top element. */
    const T& top() { return arr.front(); }

    /**
     * \brief Insert an element into the heap.
     * 
     * Compare and swap elements \f$O(\log_D N)\f$ times.
     */
    void push(const T &t) {
        arr.push_back(t);
        _sift_up();
    }

    /** \copydoc push(const T&) */
    void push(T &&t) {
        arr.push_back(std::move(t));
        _sift_up();
    }

    /** \brief Construct a new element in place in the heap.
     * \copydetails push(const T&) */
    template<typename... Args>
    void emplace(Args&&... args) {
        arr.emplace_back(std::forward<Args>(args)...);
        _sift_up();
    }

    /**
     * \brief Remove the top element from the heap.
     * 
     * The heap must be non-empty before calling this method.
     * Compare elements \f$O(D\log_D N)\f$ times and swap them \f$O(\log_D N)\f$ times.
     */
    void pop() {
        arr.front() = std::move(arr.back());
        arr.pop_back();
        _sift_down();
    }

private:
    std::vector<T> arr;
    Comp comp;

    void _sift_up() {
        size_type i = arr.size() - 1;
        while (i > 0 && comp(arr[i], arr[(i - 1) / D])) {
            std::swap(arr[i], arr[(i - 1) / D]);
            i = (i - 1) / D;
        }
    }

    void _sift_down() {
        size_type i = 0;
        while (i * D + 1 < arr.size()) {
            size_type min_child = i * D + 1;
            for (size_type child = i * D + 2; child <= std::min(i * D + D, arr.size()); child++) {
                if (comp(arr[child], arr[min_child])) {
                    min_child = child;
                }
            }
            if (comp(arr[min_child], arr[i])) {
                std::swap(arr[min_child], arr[i]);
                i = min_child;
            } else {
                break;
            }
        }
    }
};

}   // namespace cplib