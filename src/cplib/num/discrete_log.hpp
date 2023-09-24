#pragma once

#include <cmath>
#include <optional>
#include <unordered_map>

#include "cplib/num/bmint.hpp"
#include "cplib/num/gcd.hpp"
#include "cplib/port/bit.hpp"

namespace cplib {

namespace impl {

// g^k=x (mod n)
template <typename ModInt>
std::optional<typename ModInt::int_type> discrete_log_coprime_modint(ModInt g, ModInt x) {
  using T = typename ModInt::int_type;
  const T n = ModInt::mod(), m = std::ceil(std::sqrt(n));
  std::unordered_map<T, T> table;
  ModInt s(1);
  for (T i = 1; i <= m; i++) {
    s *= g;
    table.try_emplace(s.residue(), i);
  }
  s = s.inv();
  for (T i = 0; i < m; i++) {
    if (auto it = table.find(x.residue()); it != table.end()) {
      return i * m + it->second;
    }
    x *= s;
  }
  return std::nullopt;
}

// Solve y*g^k=x (mod n) naively if k<=t, otherwise return -1 and y*g^t%n.
template <typename T>
std::pair<int, T> discrete_log_naive(T g, T x, T n, int t) {
  if (x == 1) {
    return {0, x};
  }
  using bmint = BarrettModInt<DynamicBarrettReductionContext<T>>;
  auto _guard = bmint::set_mod_guard(n);
  bmint mg(g), my(1);
  for (int i = 1; i <= t; i++) {
    my *= mg;
    if (my.val() == x) {
      return {i, x};
    }
  }
  return {-1, my.val()};
}

}  // namespace impl

/**
 * \brief Modular discrete logarithm
 * \ingroup num
 *
 * Given integers \f$n\geq 1\f$ and \f$0\leq g,x <n\f$, return the minimum **non-negative** integer \f$k\f$ such that
 * \f$g^k\equiv x\f$ (mod \f$n\f$), or `std::nullopt` if no such \f$k\f$ exists.
 *
 * Note that if \f$x=1\f$ this function will always return 0. The minimum **positive** integer \f$k\f$ such that
 * \f$g^k\equiv 1\f$ (mod \f$n\f$) is called the multiplicative order of \f$g\f$ modulo \f$n\f$, and can be found more
 * efficiently than discrete logarithm.
 */
template <typename T, std::enable_if_t<std::is_unsigned_v<T>>* = nullptr>
std::optional<T> discrete_log(T g, T x, T n) {
  if (n == 1) {
    return 0;
  }
  const int t = port::bit_width(n) - 1;
  auto [k, y] = impl::discrete_log_naive(g, x, n, t);
  if (k >= 0) {
    return k;
  }
  if (y == 0) {
    return std::nullopt;
  }
  T d = gcd(y, n);
  if (x % d != 0) {
    return std::nullopt;
  }
  using mint = BarrettModInt<impl::DynamicBarrettReductionContext<T>>;
  auto _guard = mint::set_mod_guard(n / d);
  if (auto ret = impl::discrete_log_coprime_modint(mint(g), mint(x) / mint(y))) {
    return ret.value() + t;
  }
  return std::nullopt;
}

}  // namespace cplib