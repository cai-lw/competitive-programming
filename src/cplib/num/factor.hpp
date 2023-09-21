#pragma once

#include <vector>
#include <limits>
#include "cplib/port/bit.hpp"
#include "cplib/num/mmint.hpp"
#include "cplib/num/prime.hpp"

namespace cplib {

namespace impl {

template<typename T>
struct FactorizationResult {
    std::vector<T> factors, prime_factors;
};

// https://maths-people.anu.edu.au/~brent/pd/rpb051i.pdf
template<typename ModInt>
typename ModInt::int_type pollard_rho_modint() {
    using T = typename ModInt::int_type;
    const T n = ModInt::mod();
    constexpr T m = std::numeric_limits<T>::digits;
    T r = 1, g;
    ModInt c(0), y, q, x, ys;
    do {
        ++c;
        y = ModInt(2);
        q = ModInt(1);
        g = 1;
        do {
            x = y;
            for (T i = 0; i < r; i++) {
                y = y * y + c;
            }
            ys = y;
            for (T i = 0; i < r; i++) {
                y = y * y + c;
                q *= y - x;
                if ((i + 1) % m == 0) {
                    g = gcd(q.val(), n);
                    if (g != 1) {
                        break;
                    }
                    ys = y;
                }
            }
            if (g == 1 && r % m != 0) {
                g = gcd(q.val(), n);
            }
            r *= 2;
        } while (g == 1);
        if (g == n) {
            do {
                ys = ys * ys + c;
                g = gcd((ys - x).val(), n);
            } while (g == 1);
        }
    } while (g == n);
    return g;
}

template<typename T>
void factorize_work(FactorizationResult<T> &result) {
    T n = result.factors.back();
    result.factors.pop_back();
    T f = prime_or_factor(n);
    if (f == 1) {
        result.prime_factors.push_back(n);
        return;
    }
    if (f == 0) {
        if (n < (1ull << 32)) {
            f = visit_by_modulus([](auto mint) { return pollard_rho_modint<decltype(mint)>(); }, uint32_t(n));
        } else {
            f = visit_by_modulus([](auto mint) { return pollard_rho_modint<decltype(mint)>(); }, n);
        }
    }
    result.factors.push_back(f);
    result.factors.push_back(n / f);
}

}  // namespace impl

/**
 * \brief Integer factorization.
 * \ingroup num
 * 
 * Returns primes factors with multiplicity in ascending order.
 * 
 * After ruling out primes (and possibly finding a non-trivial factor) with prime_or_factor(), it runs
 * [Brent's improved version of Pollard's rho algorithm](https://maths-people.anu.edu.au/~brent/pub/pub051.html).
 * Time complexity is \f$O(N^{1/4})\f$ expected.
 * 
 * \tparam T An unsigned integer type.
 */
template<typename T, std::enable_if_t<std::is_unsigned_v<T>>* = nullptr>
std::vector<T> factorize(T n) {
    if (n <= 1) {
        return {};
    }
    int twos = port::countr_zero(n);
    impl::FactorizationResult<T> result;
    result.prime_factors.insert(result.prime_factors.end(), twos, 2);
    if (port::has_single_bit(n)) {
        return result.prime_factors;
    }
    result.factors.push_back(n >> twos);
    while (!result.factors.empty()) {
        impl::factorize_work(result);
    }
    std::sort(result.prime_factors.begin(), result.prime_factors.end());
    return result.prime_factors;
}

}  // namespace cplib