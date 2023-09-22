#pragma once

#include <algorithm>
#include <cmath>
#include <optional>

#include "cplib/num/factor.hpp"
#include "cplib/num/mmint.hpp"
#include "cplib/num/pow.hpp"
#include "cplib/num/prime.hpp"

namespace cplib {

namespace impl {

template <typename ModInt>
typename ModInt::int_type primitive_root_modint(typename ModInt::int_type phi) {
  using T = typename ModInt::int_type;
  std::vector<T> exps = factorize(phi);
  exps.erase(std::unique(exps.begin(), exps.end()), exps.end());
  for (T& e : exps) {
    e = phi / e;
  }
  for (ModInt g(2); g != ModInt(0); g++) {
    bool ok = true;
    for (T e : exps) {
      if (pow(g, e) == ModInt(1)) {
        ok = false;
        break;
      }
    }
    if (ok) {
      return g.val();
    }
  }
  return 0;  // unreachable
}

template <typename ModInt>
typename ModInt::int_type primitive_root_unfactorized_modint() {
  using T = typename ModInt::int_type;
  T n = ModInt::mod();
  if (is_prime(n)) {
    return primitive_root_modint<ModInt>(n - 1);
  }
  T b2 = std::round(std::sqrt(n));
  if (is_prime(b2) && b2 * b2 == n) {
    return primitive_root_modint<ModInt>(n / b2 * (b2 - 1));
  }
  T b3 = std::round(std::cbrt(n));
  if (is_prime(b3) && b3 * b3 * b3 == n) {
    return primitive_root_modint<ModInt>(n / b3 * (b3 - 1));
  }
  for (int e = 4;; e++) {
    T b = std::round(std::pow(n, 1.0 / e));
    if (b < 23) {
      break;
    }
    if (is_prime(b) && ::cplib::pow(b, e) == n) {
      return primitive_root_modint<ModInt>(n / b * (b - 1));
    }
  }
  for (int p : {3, 5, 7, 11, 13, 17, 19}) {
    if (p * p * p > n) {
      break;
    }
    double l = std::log(n) / std::log(p);
    int e = std::round(l);
    if (std::abs(e - l) < 1e-9 && ::cplib::pow(p, e) == n) {
      return primitive_root_modint<ModInt>(n / p * (p - 1));
    }
  }
  return 0;
}

}  // namespace impl

/**
 * \brief Primitive root modulo a prime number.
 * \ingroup num
 *
 * For the given prime \f$p\f$, returns any \f$0<g<p\f$ such that the minimum positive integer \f$k\f$ that satisfies
 * \f$g^k\equiv 1 \pmod{p}\f$ is \f$p-1\f$.
 */
template <typename T, std::enable_if_t<std::is_unsigned_v<T>>* = nullptr>
T primitive_root_prime(T p) {
  if (p == 2) {
    // Cannot use MontgomeryModInt since 2 is even.
    return 1;
  }
  return visit_by_modulus([&](auto mint) { return impl::primitive_root_modint<decltype(mint)>(p - 1); }, p);
}

/**
 * \brief Primitive root modulo any number.
 * \ingroup num
 *
 * For any primitive root \f$g\f$ the minimum positive integer \f$k\f$ that satisfies \f$g^k\equiv 1 \pmod{n}\f$ is
 * \f$\phi(n)\f$ where \f$\phi\f$ is Euler's totient function. Primitive root exists if and only if \f$n=2,4,p^k,2p^k\f$
 * where \f$p\f$ is an odd prime and \f$k\geq 1\f$. This function returns `std::nullopt` if it does not exist.
 */
template <typename T, std::enable_if_t<std::is_unsigned_v<T>>* = nullptr>
std::optional<T> primitive_root(T n) {
  if (n <= 1) {
    return std::nullopt;
  }
  if (n == 2 || n == 4) {
    return n - 1;
  }
  bool even = false;
  if (n % 2 == 0) {
    even = true;
    n /= 2;
  }
  if (n % 2 == 0) {
    return std::nullopt;
  }
  T g = visit_by_modulus([&](auto mint) { return impl::primitive_root_unfactorized_modint<decltype(mint)>(); }, n);
  if (g == 0) {
    return std::nullopt;
  }
  if (even && g % 2 == 0) {
    g += n;
  }
  return g;
}

}  // namespace cplib