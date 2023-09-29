#pragma once

#include <cstdint>
#include <functional>

namespace cplib {

/**
 * \brief A generic exponetiation by squaring function
 * \ingroup num
 * \tparam Op An associative binary operator over `T`
 */
template <typename T, typename Op = std::multiplies<T>>
constexpr T pow(T base, uint64_t exp, Op&& op = {}) {
  T res(1);
  while (exp) {
    if (exp & 1) {
      res = op(res, base);
    }
    base = op(base, base);
    exp >>= 1;
  }
  return res;
}

}  // namespace cplib