#pragma once

#include "cplib/port/bit.hpp"
#include "cplib/num/gcd.hpp"
#include "cplib/num/mmint.hpp"
#include "cplib/num/pow.hpp"

namespace cplib {

namespace impl {

template<typename ModInt, typename T = typename ModInt::int_type>
T miller_rabin(ModInt a, T d, int r) {
    const ModInt one(1), minus_one(-1);
    ModInt x = pow(a, d);
    if (x == one || x == minus_one) {
        return 1;
    }
    while (r--) {
        ModInt y = x * x;
        if (y == one) {
            return gcd(x.val() - 1, ModInt::mod());
        }
        x = y;
        if (x == minus_one) {
            return 1;
        }
    }
    return 0;
}

constexpr uint64_t small_primes_mask() {
    uint64_t mask = 0;
    for (int i : {2, 3 ,5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61}) {
        mask |= 1ull << i;
    }
    return mask;
}

constexpr bool is_prime_lt64(int n) {
    return (1ull << n) & small_primes_mask();
}

static uint32_t prime_or_factor_32(uint32_t n) {
    if (n < 64) {
        return is_prime_lt64(n);
    }
    if (n % 2 == 0) {
        return 2;
    }
    constexpr uint32_t small_prod = 3u * 5 * 7 * 11 * 13 * 17 * 19 * 23 * 29;
    uint32_t g = gcd(n, small_prod);
    if (g != 1) {
        return g != n ? g : 0;
    }
    using ctx = DynamicMontgomeryReductionContext<uint32_t>;
    auto _guard = ctx::set_mod(n);
    using mint = MontgomeryModInt<ctx>;
    int r = port::countr_zero(n - 1);
    uint32_t d = (n - 1) >> r;
    for (uint32_t a : {2, 7, 61}) {
        uint32_t ret = miller_rabin(mint(a), d, r);
        if (ret != 1) {
            return ret;
        }
    }
    return 1;
}

static uint64_t prime_or_factor_64(uint64_t n) {
    if (n < 64) {
        return is_prime_lt64(n);
    }
    if (n % 2 == 0) {
        return 2;
    }
    constexpr uint64_t small_prod = 3ull * 5 * 7 * 11 * 13 * 17 * 19 * 23 * 29 * 31 * 37 * 41 * 43 * 47 * 53;
    uint64_t g = gcd(n, small_prod);
    if (g != 1) {
        return g != n ? g : 0;
    }
    using ctx = DynamicMontgomeryReductionContext<uint64_t>;
    auto _guard = ctx::set_mod(n);
    using mint = MontgomeryModInt<ctx>;
    int r = port::countr_zero(n - 1);
    uint64_t d = (n - 1) >> r;
    for (uint64_t a : {2, 325, 9375, 28178, 450775, 9780504, 1795265022}) {
        uint64_t ret = miller_rabin(mint(a), d, r);
        if (ret != 1) {
            return ret;
        }
    }
    return 1;
}

}  // namespace impl

/**
 * \brief Primality test and possibly return a non-trivial factor.
 * \ingroup num
 * \see is_prime() Discards the factor and returns a boolean only.
 * 
 * Always returns 1 if `n` is prime. Otherwise, may return 0 or a non-trivial factor of `n`. As most factorization
 * algorithm requires primality test first, a factor found in primality test is work saved for factorization.
 * 
 * In this implementation, after ruling out small prime divisors, Miller-Rabin test is run on a fixed set of \f$k\f$
 * bases that are known to cover all numbers up to a certain bound, where \f$k=3\f$ covers all 32-bit integers and
 * \f$k=7\f$ covers all 64-bit integers. The time complexity is thus \f$O(k\log N)\f$.
 * 
 * In this implementation, the non-trivial factor may come from one of the following:
 * * 2 if `n` is even.
 * * GCD of `n` and product of small odd primes.
 * * [Finding factors in Miller-Rabin test](https://en.wikipedia.org/wiki/Miller%E2%80%93Rabin_primality_test#Variants_for_finding_factors).
 * 
 * \tparam T An unsigned integer type.
 * \param n Integer no larger than \f$2^{62}-1\f$, the largest modulus allowed by MontgomeryModInt, which is used in
 * this implementation.
 */
template<typename T, std::enable_if_t<std::is_unsigned_v<T>>* = nullptr>
T prime_or_factor(T n) {
    if (n < (1 << 30)) {
        return impl::prime_or_factor_32(n);
    } else {
        return impl::prime_or_factor_64(n);
    }
}

/**
 * \brief Primality test.
 * \ingroup num
 * \see prime_or_factor() Implementation details.
 * 
 * \tparam T An unsigned integer type.
 * \param n Integer no larger than \f$2^{62}-1\f$, the largest modulus allowed by MontgomeryModInt, which is used in
 * this implementation.
 */
template<typename T, std::enable_if_t<std::is_unsigned_v<T>>* = nullptr>
bool is_prime(T n) {
    return prime_or_factor(n) == 1;
}

}  // namespace cplib