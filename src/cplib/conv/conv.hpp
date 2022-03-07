#pragma once

#include <vector>
#include "cplib/port/bit.hpp"
#include "cplib/conv/fft.hpp"

namespace cplib {

namespace impl {

template<typename T>
void conv_naive_inplace2(std::vector<T> &a, std::vector<T> &b) {
    using usize = std::size_t;
    usize out_size = a.size() + b.size() - 1;
    a.resize(out_size, T(0));
    usize k = out_size;
    do {
        --k;
        usize i_begin = k >= b.size() ? k + 1 - b.size() : 0;
        usize i_end = std::min(k, a.size());
        a[k] *= b[0];
        for (usize i = i_begin; i < i_end; ++i) {
            a[k] += a[i] * b[k - i];
        }
    } while(k != 0);
}

template<typename T>
void conv_fft_inplace2(std::vector<T> &a, std::vector<T> &b) {
    using usize = std::size_t;
    usize out_size = a.size() + b.size() - 1;
    usize padded_out_size = port::bit_ceil(out_size);
    a.resize(padded_out_size, T(0));
    b.resize(padded_out_size, T(0));
    fft_inplace(a);
    fft_inplace(b);
    for (size_t i = 0; i < padded_out_size; i++) {
        a[i] *= b[i];
    }
    ifft_inplace(a);
    a.resize(out_size);
}

}  // namespace impl

/**
 * \brief In-place convolution where both arrays are modified.
 * \ingroup conv
 * 
 * The convolution of `a` and `b` is stored in `a`, with length `a.size() + b.size() - 1`, unless at least one of the
 * array is empty, in which case the output is empty as well. The result length must be no larger than the largest
 * \f$2^n\f$ for which `T` has \f$2^n\f$-th root of unity.
 * 
 * `b` is modified in an unspecified way. Use convolve_inplace() if `b` needs to be preserved for later use, or
 * convolve() if both `a` and `b` need to be preserved.
 * 
 * \tparam T See fft_inplace() for requirements for `T`.
 */
template<typename T>
void convolve_inplace2(std::vector<T> &a, std::vector<T> &b) {
    if (a.empty() || b.empty()) {
        a.clear();
    } else if (std::min(a.size(), b.size()) <= 32) {
        impl::conv_naive_inplace2(a, b);
    } else {
        impl::conv_fft_inplace2(a, b);
    }
}

/**
 * \brief In-place convolution where one array is modified.
 * \ingroup conv
 * 
 * The convolution of `a` and `b` is stored in `a`.
 * 
 * \see convolve_inplace2() for details
 */
template<typename T>
void convolve_inplace(std::vector<T> &a, const std::vector<T> &b) {
    auto b_copy = b;
    convolve_inplace2(a, b_copy);
}

/**
 * \brief Returns the convolution of two arrays.
 * \ingroup conv
 * \see convolve_inplace2() for details.
 */
template<typename T>
std::vector<T> convolve(const std::vector<T> &a, const std::vector<T> &b) {
    auto a_copy = a;
    convolve_inplace(a_copy, b);
    return a_copy;
}

}  // namespace cplib