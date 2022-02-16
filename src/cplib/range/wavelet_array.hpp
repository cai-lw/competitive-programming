#pragma once

#include <array>
#include <iterator>
#include <vector>
#include "cplib/range/bit_dict.hpp"

namespace cplib {

/**
 * \brief Efficient representation of a wavelet tree, supporting various static range queries.
 * 
 * Each level of the wavelet tree is stored as a BitDict.
 * 
 * \tparam T Type of elements. Must be an unsigned integer type.
 * \tparam M Specify that all elements are integers in \f$[0,2^M-1]\f$. It is propotional to the complexity of
 * most operations and should be set as small as possible.
 */
template <typename T, int M = std::numeric_limits<T>::digits>
struct WaveletArray {
    static_assert(M > 0 && M <= std::numeric_limits<T>::digits);

    static constexpr T max_value = M == std::numeric_limits<T>::digits ? std::numeric_limits<T>::max() : (T(1) << M) - 1;
    using size_type = BitDict::size_type;

    /** \brief Creates an empty WaveletArray */
    WaveletArray() {}

    /** \brief Creates a WaveletArray by consuming an array of elements. */
    WaveletArray(std::vector<T>&& data) {
        std::vector<T> temp = std::move(data);
        *this = build_and_sort(temp.begin(), temp.end());
    }

    /**
     * \brief Builds a WaveletArray from a range of elements, sorting them in place as a side effect.
     * 
     * This is the most generic interface for constructing a WaveletArray. The side effect is rarely desirable and
     * the original data is usually discarded afterwards. Runs in \f$O(NM)\f$ time.
     * 
     * \tparam BidirIt A bidirectional iterator type.
     */
    template <typename BidirIt>
    static WaveletArray build_and_sort(BidirIt first, BidirIt last) {
        WaveletArray<T, M> wa;
        std::size_t size = std::distance(first, last);
        for (int lvl = M - 1; lvl >= 0; lvl--) {
            BitDictBuilder builder(size);
            BitDict::size_type i = 0;
            for (auto it = first; it != last; ++it) {
                builder.set(i++, (*it >> lvl) & 1);
            }
            wa.bit_dict[lvl] = builder.build();
            stable_partition(first, last, [=](T x) { return !((x >> lvl) & 1); });
        }
        return wa;
    }

    /** \brief Returns the number of elements. */
    size_type size() { return bit_dict[0].size(); }

    /** \brief Returns the element at the given index. */
    T get(size_type idx) {
        T ret = 0;
        for (int lvl = M - 1; lvl >= 0; lvl--) {
            bool bit = bit_dict[lvl][idx];
            ret |= T(bit) << lvl;
            idx = bit_dict[lvl].rank_to_child(idx, bit);
        }
        return ret;
    }

    /**
     * \brief Returns the 0-indexed n-th smallest element in the range `[left, right)`.
     * 
     * Requires `0 <= left < right <= size()` and `0 <= n < right - left`.
     * Calls \f$O(M)\f$ BitDict operations.
     */
    T range_nth(size_type left, size_type right, size_type n) {
        T ret = 0;
        for (int lvl = M - 1; lvl >= 0; lvl--) {
            const BitDict &bd = bit_dict[lvl];
            size_type zero_count = bd.rank0(right) - bd.rank0(left);
            bool bit = n >= zero_count;
            ret |= T(bit) << lvl;
            if (bit) {
                n -= zero_count;
            }
            left = bd.rank_to_child(left, bit);
            right = bd.rank_to_child(right, bit);
        }
        return ret;
    }

    /**
     * \brief Returns the number of the given value in the range `[left, right)`.
     * 
     * Requires `0 <= left < right <= size()` and `0 <= value <= max_value`.
     * Calls \f$O(M)\f$ BitDict operations.
     */
    size_type range_count(size_type left, size_type right, T val) {
        for (int lvl = M - 1; lvl >= 0; lvl--) {
            bool bit = (val >> lvl) & 1;
            left = bit_dict[lvl].rank_to_child(left, bit);
            right = bit_dict[lvl].rank_to_child(right, bit);
        }
        return right - left;
    }

    /**
     * \brief Returns the number of elements in the range `[left, right)` whose values are between `[low, high]`.
     * 
     * Requires `0 <= left < right <= size()` and `0 <= low <= high <= max_value`. Note that the value range is
     * **inclusive**, to allow using the maximum value of the type `T`. Calls \f$O(M)\f$ BitDict operations.
     */
    size_type range_count_between(size_type left, size_type right, T low, T high) {
        return _rangefreq(left, right, low, high, M - 1);
    }

private:
    std::array<BitDict, M> bit_dict;

    size_type _rangefreq(size_type left, size_type right, T low, T high, int lvl) {
        if (left >= right) {
            return 0;
        } else if (high - low == (lvl == M - 1 ? max_value : (T(1) << (lvl + 1)) - 1)) {
            return right - left;
        }
        T bit_mask = T(1) << lvl;
        const BitDict &bd = bit_dict[lvl];
        if (bit_mask & (low ^ high)) {
            T split = ~(bit_mask - 1) & high;
            return _rangefreq(bd.rank_to_child(left, false), bd.rank_to_child(right, false), low, split - 1, lvl - 1)
                 + _rangefreq(bd.rank_to_child(left, true), bd.rank_to_child(right, true), split, high, lvl - 1);
        } else {
            bool bit = bit_mask & low;
            return _rangefreq(bd.rank_to_child(left, bit), bd.rank_to_child(right, bit), low, high, lvl - 1);
        }
    }
};

}  // namespace cplib