#pragma once

#include <vector>

#include "cplib/conv/fft.hpp"
#include "cplib/port/bit.hpp"

namespace cplib {

namespace impl {

inline bool conv_naive_is_efficient(std::size_t n, std::size_t m) { return std::min(n, m) <= 32; }

template <typename T>
void conv_naive_inplace(std::vector<T> &a, const std::vector<T> &b) {
  if (a.empty() || b.empty()) {
    a.clear();
    return;
  }
  using usize = std::size_t;
  usize a_deg = a.size() - 1, b_deg = b.size() - 1;
  a.resize(a_deg + b_deg + 1, T(0));
  for (usize i = a_deg + b_deg; i > 0; i--) {
    if (i <= a_deg) {
      a[i] *= b[0];
    }
    usize j_low = i <= a_deg ? 1 : i - a_deg;
    usize j_high = i <= b_deg ? i : b_deg;
    for (usize j = j_low; j <= j_high; j++) {
      a[i] += a[i - j] * b[j];
    }
  }
  a[0] *= b[0];
}

template <typename T>
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
template <typename T>
void convolve_inplace2(std::vector<T> &a, std::vector<T> &b) {
  if (impl::conv_naive_is_efficient(a.size(), b.size())) {
    impl::conv_naive_inplace(a, b);
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
template <typename T>
void convolve_inplace(std::vector<T> &a, const std::vector<T> &b) {
  if (impl::conv_naive_is_efficient(a.size(), b.size())) {
    impl::conv_naive_inplace(a, b);
  } else {
    auto b_copy = b;
    impl::conv_fft_inplace2(a, b_copy);
  }
}

/**
 * \brief Returns the convolution of two arrays.
 * \ingroup conv
 * \see convolve_inplace2() for details.
 */
template <typename T>
std::vector<T> convolve(const std::vector<T> &a, const std::vector<T> &b) {
  auto a_copy = a;
  convolve_inplace(a_copy, b);
  return a_copy;
}

}  // namespace cplib