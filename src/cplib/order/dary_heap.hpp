#pragma once

#include <functional>
#include <vector>

namespace cplib {

namespace impl {

template<typename T, typename Comp = std::less<T>, int D = 8>
struct DaryHeapNormalImpl {

};

}

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

    DaryHeap() : arr_(), comp_() {}

    /** \brief Returns the number of elements. */
    size_type size() const { return arr_.size(); }

    /** \brief Returns whether the heap is empty. */
    bool empty() const { return arr_.empty(); }

    /** \brief Returns the top element. */
    const T& top() { return arr_.front(); }

    /**
     * \brief Insert an element into the heap.
     * 
     * Compare and swap elements \f$O(\log_D N)\f$ times.
     */
    void push(const T &t) {
        arr_.push_back(t);
        sift_up_();
    }

    /** \copydoc push(const T&) */
    void push(T &&t) {
        arr_.push_back(std::move(t));
        sift_up_();
    }

    /** \brief Construct a new element in place in the heap.
     * \copydetails push(const T&) */
    template<typename... Args>
    void emplace(Args&&... args) {
        arr_.emplace_back(std::forward<Args>(args)...);
        sift_up_();
    }

    /**
     * \brief Remove the top element from the heap.
     * 
     * The heap must be non-empty before calling this method.
     * Compare elements \f$O(D\log_D N)\f$ times and swap them \f$O(\log_D N)\f$ times.
     */
    void pop() {
        arr_.front() = std::move(arr_.back());
        arr_.pop_back();
        sift_down_();
    }

private:
    std::vector<T> arr_;
    Comp comp_;

    void sift_up_() {
        size_type i = arr_.size() - 1;
        while (i > 0 && comp_(arr_[i], arr_[(i - 1) / D])) {
            std::swap(arr_[i], arr_[(i - 1) / D]);
            i = (i - 1) / D;
        }
    }

    void sift_down_() {
        size_type i = 0;
        while (i * D + 1 < arr_.size()) {
            size_type min_child = i * D + 1;
            for (size_type child = i * D + 2; child < std::min(i * D + (D + 1), arr_.size()); child++) {
                if (comp_(arr_[child], arr_[min_child])) {
                    min_child = child;
                }
            }
            if (comp_(arr_[min_child], arr_[i])) {
                std::swap(arr_[min_child], arr_[i]);
                i = min_child;
            } else {
                break;
            }
        }
    }
};

}   // namespace cplib