#pragma once

#include "cplib/conv/fft.hpp"

namespace cplib {

/**
 * \brief Multiply two multivariate formal power series (FPS).
 * \ingroup conv
 *
 * Returns the product of two multivariate FPSs, with both inputs and the output truncated to the same shape. Formally,
 * this returns \f$H\f$ where
 * \f[
 * H(x_1,x_2,\dots,x_k)\equiv F(x_1,x_2,\dots,x_k)G(x_1,x_2,\dots,x_k)\pmod{(x_1^{n_1},x_2^{n_2},\dots ,x_k^{n_k})}
 * \f]
 *
 * Coefficients of \f$F,G,H\f$ are given as \f$n_1\times n_2\times \dots\times n_k\f$ multi-dimensional arrays,
 * but flattened into one dimension in row-major order. That is, the subscript for the coefficient of the term
 * \f$x_1^{i_1}x_2^{i_2}\dots x_k^{i_k}\f$ is \f$i=i_1n_2n_3\dots n_k+i_2n_3\dots n_k+\dots+i_{k-1}n_k+i_k\f$.
 * `shape` is simply the sequence \f$\{n_1,n_2,\dots,n_k\}\f$.
 *
 * Time complexity is \f$O(kN\log N)\f$ where \f$N=n_1n_2\dots n_k\f$, and space complexity is \f$O(kN)\f$.
 * The algorithm comes from https://rushcheyo.blog.uoj.ac/blog/6547 (Chinese).
 *
 * \tparam T See fft_inplace() for requirements for `T`.
 */
template <typename T>
std::vector<T> multiply_multivar_fps(const std::vector<T>& a, const std::vector<T>& b,
                                     const std::vector<std::size_t>& shape) {
  using usize = std::size_t;
  std::vector<usize> squeezed_shape;
  usize n = 1;
  for (usize dim : shape) {
    n *= dim;
    if (dim > 1) {
      squeezed_shape.push_back(dim);
    }
  }
  assert(n == a.size() && n == b.size());
  usize ndims = squeezed_shape.size();
  if (ndims == 0) {
    return {a[0] * b[0]};
  }
  for (usize d = ndims - 1; d > 0; d--) {
    squeezed_shape[d - 1] *= squeezed_shape[d];
  }
  usize padded_out_size = port::bit_ceil(n * 2 - 1);
  std::vector<std::vector<T>> a_ranked(ndims, std::vector<T>(padded_out_size, T(0)));
  std::vector<std::vector<T>> b_ranked(ndims, std::vector<T>(padded_out_size, T(0)));
  for (usize i = 0; i < n; i++) {
    usize rank = 0;
    for (usize d = ndims - 1; d > 0; d--) {
      rank += i / squeezed_shape[d];
    }
    rank %= ndims;
    a_ranked[rank][i] = a[i];
    b_ranked[rank][i] = b[i];
  }
  for (usize r = 0; r < ndims; r++) {
    fft_inplace(a_ranked[r]);
    fft_inplace(b_ranked[r]);
  }
  std::vector<T> prod(ndims);
  for (usize i = 0; i < padded_out_size; i++) {
    fill(prod.begin(), prod.end(), T(0));
    for (usize r1 = 0; r1 < ndims; r1++) {
      for (usize r2 = 0; r2 < ndims; r2++) {
        usize r = r1 + r2;
        if (r >= ndims) {
          r -= ndims;
        }
        prod[r] += a_ranked[r1][i] * b_ranked[r2][i];
      }
    }
    for (usize r = 0; r < ndims; r++) {
      a_ranked[r][i] = prod[r];
    }
  }
  for (usize r = 0; r < ndims; r++) {
    ifft_inplace(a_ranked[r]);
  }

  std::vector<T> out(n, T(0));
  for (usize i = 0; i < n; i++) {
    usize rank = 0;
    for (usize d = ndims - 1; d > 0; d--) {
      rank += i / squeezed_shape[d];
    }
    rank %= ndims;
    out[i] = a_ranked[rank][i];
  }
  return out;
}

}  // namespace cplib