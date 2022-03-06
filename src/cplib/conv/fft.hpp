#pragma once

#include <cmath>
#include <complex>
#include <iterator>
#include <vector>
#include "cplib/port/bit.hpp"
#include "cplib/num/mmint.hpp"
#include "cplib/num/pow.hpp"

namespace cplib {

namespace impl {

template<typename T>
std::vector<T> twiddling_factors(const std::vector<T> &roots) {
    std::size_t n = 1 << (roots.size() - 2);
    std::vector<T> w, w_stack;
    w.reserve(n);
    w_stack.reserve(roots.size());
    w.push_back(roots.front());
    w_stack.push_back(roots.front());
    // For any give i, the k-th element in the stack is i with only the highest k 1-bits.
    // Each element is the product of its lowest bit and other bits, and thus is the product of at most log2(N) roots.
    // Thus for complex numbers the error in twiddling factors is at most log2(N) eps.
    for (std::size_t i = 1; i < n; i++) {
        w_stack.resize(port::popcount(i));
        w_stack.push_back(w_stack.back() * roots[roots.size() - 1 - port::countr_zero(i)]);
        w.push_back(w_stack.back());
    }
    return w;
}

}  // namespace impl

/**
 * \brief \f$2^n\f$-th root of unity for radix-2 FFT.
 * \ingroup conv
 * \see radix2_fft_root<MMInt<998244353>>, radix2_fft_root<MMInt64<4179340454199820289>>,
 * radix2_fft_root<std::complex<Float>>
 * 
 * Specialization of this class must provide a method `static T get(int n)` that returns the \f$2^n\f$-th root of unity.
 * That is, \f$\omega_{2^n}\f$ such that \f$\omega_{2^n}^{2^n}=1\f$ but \f$\omega_{2^n}^{2^{n-1}}\neq 1\f$, where
 * \f$1\f$ is the multiplicative identity. Specifically, for \f$n=0\f$ we define \f$\omega_{2^0}=1\f$.
 * 
 * Although \f$\omega_{2^n}\f$ is generally not unique, the method must always return the same one for each \f$n\f$.
 * Additionally, for all \f$n>0\f$ that \f$\omega_{2^n}\f$ exists, \f$\omega_{2^n}^2=\omega_{2^{n-1}}\f$ must hold.
 * 
 * The specialization can have undefined behavior if \f$\omega_{2^n}\f$ doesn't exist.
 * 
 * \tparam T A commutative ring on which the \f$2^n\f$-th root of unity can be defined as above.
 */
template<typename T>
struct radix2_fft_root {};

/**
 * \brief Specialization of radix2_fft_root for \f$\mathbb{F}_{998244353}\f$, the most common FFT-friendly
 * finite prime field in competitive programming.
 * 
 * Since \f$998244353=119\cdot 2^{23}+1\f$, \f$2^n\f$-th root of unity exists for \f$0\leq n \leq 23\f$.
 */
template<>
struct radix2_fft_root<MMInt<998244353>> {
    using mint = MMInt<998244353>;
    static mint get(int n) {
        return pow(mint(3), 119 << (23 - n));
    }
};

/**
 * \brief Specialization of radix2_fft_root for a large FFT-friendly finite prime field.
 * 
 * Since \f$p=4179340454199820289=29\cdot 2^{57}+1\f$, \f$2^n\f$-th root of unity exists for \f$0\leq n \leq 57\f$.
 * 
 * This is useful for convolution over \f$\mathbb{Z}\f$ where all terms in the result are bounded by a range smaller
 * than \f$p\approx 4\times 10^{18}\f$, so the result modulo \f$p\f$ uniquely determines the result in \f$\mathbb{Z}\f$.
 */
template<>
struct radix2_fft_root<MMInt64<4179340454199820289>> {
    using mint = MMInt64<4179340454199820289>;
    static mint get(int n) {
        return pow(mint(3), 29ull << (57 - n));
    }
};

/**
 * \brief Specialzation of radix2_fft_root for complex numbers.
 * 
 * \f$2^n\f$-th root of unity exists for any \f$n\f$.
 */
template<typename Float>
struct radix2_fft_root<std::complex<Float>> {
    constexpr static std::complex<Float> get(size_t n) {
        constexpr long double tau = atanl(1) * 8;  // In C++20, std::pi_v<Float> * 2
        return std::polar<Float>(1, tau / (1ull << n));
    }
};

/**
 * \brief In-place fast Fourier transform (FFT).
 * \ingroup conv
 * 
 * The input length must be a power of two. The output is in bit-reversed order.
 * 
 * Let the input length be \f$2^L\f$, and `T` be `std::iterator_traits<RandomIt>::value_type`, then a specialization of
 * radix2_fft_root must exist for `T` and must provide \f$2^n\f$-th root of unity for all \f$0\leq n \leq L\f$.
 * 
 * \tparam RandomIt Random-access iterator type. 
 */
template<typename RandomIt>
void fft_inplace(RandomIt first, RandomIt last) {
    using usize = std::size_t;
    const usize n = std::distance(first, last);
    assert(port::has_single_bit(n));
    int log2n = port::countr_zero(n);
    using T = typename std::iterator_traits<RandomIt>::value_type;
    std::vector<T> roots(log2n + 1);
    for (int i = 0; i <= log2n; i++) {
        roots[i] = radix2_fft_root<T>::get(i);
    }
    for (int stage = log2n - 1; stage >= 0; stage--) {
        usize len = usize(1) << stage;
        std::vector<T> w = impl::twiddling_factors(roots);
        for (usize block = 0; block < n; block += len * 2) {
            for (usize offset = 0; offset < len; offset++) {
                usize i = block + offset, j = i + len;
                T tmp = (first[i] - first[j]) * w[offset];
                first[i] += first[j];
                first[j] = tmp;
            }
        }
        roots.pop_back();
    }
}

/**
 * \brief In-place inverse fast Fourier transform (IFFT).
 * \ingroup conv
 * 
 * Exactly undoes fft_inplace(). The input length must be a power of two, and must be in bit-reversed order.
 * 
 * Let the input length be \f$2^L\f$, and `T` be `std::iterator_traits<RandomIt>::value_type`, then a specialization of
 * radix2_fft_root must exist for `T` and must provide \f$2^n\f$-th root of unity for all \f$0\leq n \leq L\f$.
 * In addition, the multiplicative inverse of \f$2\f$ must exist in `T`.
 * 
 * \tparam RandomIt Random-access iterator type. 
 */
template<typename RandomIt>
void ifft_inplace(RandomIt first, RandomIt last) {
    using usize = std::size_t;
    const usize n = std::distance(first, last);
    assert(port::has_single_bit(n));
    int log2n = port::countr_zero(n);
    using T = typename std::iterator_traits<RandomIt>::value_type;
    T one = radix2_fft_root<T>::get(0);
    std::vector<T> roots{one};
    for (int stage = 0; stage < log2n; stage++) {
        usize len = usize(1) << stage;
        roots.push_back(one / radix2_fft_root<T>::get(stage + 1));
        std::vector<T> w = impl::twiddling_factors(roots);
        for (usize block = 0; block < n; block += len * 2) {
            for (usize offset = 0; offset < len; offset++) {
                usize i = block + offset, j = i + len;
                first[j] *= w[offset];
                T tmp = first[i] - first[j];
                first[i] += first[j];
                first[j] = tmp;
            }
        }
    }
    T half = one / (one + one);
    T n_inv = pow(half, log2n);
    for (usize i = 0; i < n; i++) {
        first[i] *= n_inv;
    }
}

/**
 * \brief In-place fast Fourier transform (FFT).
 * \ingroup conv
 * 
 * The input length must be a power of two. The output is in bit-reversed order.
 * 
 * Let the input length be \f$2^L\f$, then a specialization of radix2_fft_root must exist for `T` and must provide
 * \f$2^n\f$-th root of unity for all \f$0\leq n \leq L\f$.
 */
template<typename T>
void fft_inplace(std::vector<T>& a) {
    fft_inplace(a.begin(), a.end());
}

/**
 * \brief In-place inverse fast Fourier transform (IFFT).
 * \ingroup conv
 * 
 * Exactly undoes fft_inplace(). The input length must be a power of two, and must be in bit-reversed order.
 * 
 * Let the input length be \f$2^L\f$, then a specialization of radix2_fft_root must exist for `T` and must provide
 * \f$2^n\f$-th root of unity for all \f$0\leq n \leq L\f$. In addition, the multiplicative inverse of \f$2\f$
 * must exist in `T`.
 */
template<typename T>
void ifft_inplace(std::vector<T> &a) {
    ifft_inplace(a.begin(), a.end());
}

}  // namespace cplib