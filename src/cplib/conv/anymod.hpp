#pragma once

#include <vector>

#include "cplib/conv/conv.hpp"

namespace cplib {

namespace impl {

template <typename In, typename Out>
std::vector<Out> convolve_modint(const std::vector<In>& a, const std::vector<In>& b) {
  std::vector<Out> a_modint, b_modint;
  a_modint.reserve(a.size());
  for (const In& x : a) {
    a_modint.emplace_back(x.val());
  }
  b_modint.reserve(b.size());
  for (const In& x : b) {
    b_modint.emplace_back(x.val());
  }
  convolve_inplace2(a_modint, b_modint);
  return a_modint;
}

template <typename In, typename Out, typename ModInt1, typename ModInt2>
std::vector<Out> convolve_with_two_modints(const std::vector<In>& a, const std::vector<In>& b) {
  std::vector<ModInt1> m1 = convolve_modint<In, ModInt1>(a, b);
  std::vector<ModInt2> m2 = convolve_modint<In, ModInt2>(a, b);
  std::vector<Out> ret;
  ret.reserve(m1.size());
  ModInt2 p1_inv = ModInt2(ModInt1::mod()).inv();
  Out p1_out(ModInt1::mod());
  for (std::size_t i = 0; i < m1.size(); i++) {
    // r1+k*p1=r2 (mod p2) => k=(r2-r1)*p1^{-1} (mod p2)
    auto r1 = m1[i].val();
    auto k = ((m2[i] - ModInt2(r1)) * p1_inv).val();
    ret.push_back(Out(r1) + Out(k) * p1_out);
  }
  return ret;
}

}  // namespace impl

template <>
struct radix2_fft_root<MMInt64<4242390848983007233>> {
  using mint = MMInt64<4242390848983007233>;
  static mint get(int n) { return pow(mint(11), 471ull << (53 - n)); }
};

/**
 * \brief In-place convolution with arbitrary modulus.
 * \ingroup conv
 *
 * Using two 64-bit FFT-friendly prime moduli, it effectively computes convolution modulo a large number,
 * \f$M\approx 1.9\times 10^{37}\f$. When convolution modulo \f$P\f$ is interpreted as convolution over \f$\mathbb{N}\f$
 * followed by modulo, the intermediate value is at most \f$(P-1)^2\min\{N_1,N_2\}\f$, where \f$N_1,N_2\f$ are the
 * lengths of the two sequences. As long as this value is smaller than \f$M\f$, computation modulo \f$M\f$ gives
 * the unique and correct result.
 *
 * Typically in competitive programming, \f$P\approx 10^9\f$ and \f$N_1,N_2\lesssim 10^6\f$, so \f$M\f$ only
 * needs to be larger than \f$10^{25}\f$ or so. Arbitrary modulus convolution is more commonly implemented as three
 * 32-bit moduli, as they combined provide a sufficiently large \f$M\f$. However, benchmark shows that two 64-bit
 * moduli is about as fast as three 32-bit moduli on 64-bit platforms.
 *
 * \tparam ModInt A modint type. The only requirements are `operator+`, `opeartor*`, and conversion from `uint64_t`.
 */
template <typename ModInt>
void convolve_any_modint_inplace(std::vector<ModInt>& a, const std::vector<ModInt>& b) {
  if (impl::conv_naive_is_efficient(a.size(), b.size())) {
    impl::conv_naive_inplace(a, b);
    return;
  }
  using mint1 = MMInt64<4512606826625236993>;
  using mint2 = MMInt64<4242390848983007233>;
  using u128 = unsigned __int128;
  u128 max_prod = u128(ModInt::mod() - 1) * (ModInt::mod() - 1);
  u128 limit = u128(mint1::mod()) * mint2::mod() - 1;
  assert(max_prod <= limit / std::min(a.size(), b.size()));
  a = impl::convolve_with_two_modints<ModInt, ModInt, mint1, mint2>(a, b);
}

/**
 * \brief Returns the convolution of two arrays modulo an arbitrary integer.
 * \ingroup conv
 * \see convolve_any_modint_inplace() for details.
 */
template <typename ModInt>
std::vector<ModInt> convolve_any_modint(const std::vector<ModInt>& a, const std::vector<ModInt>& b) {
  std::vector<ModInt> a_copy = a;
  convolve_any_modint_inplace(a_copy, b);
  return a_copy;
}

}  // namespace cplib