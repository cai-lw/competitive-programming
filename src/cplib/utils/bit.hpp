#pragma once

#include <x86intrin.h>

#include "cplib/port/bit.hpp"

#pragma GCC push_options
#ifndef _CPLIB_NO_FORCE_BMI2_
#pragma GCC target("abm,bmi,bmi2")
#endif

namespace cplib::impl {

// Manually inserting BZHI intrinsic because GCC won't optimize x & ((1ull << n) - 1) to BZHI until version 10.
// See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=93346
inline uint64_t low_bits(uint64_t x, int n) {
#if __GNUC__ < 10 && (defined(__BMI2__) || !defined(_CPLIB_NO_FORCE_BMI2_))
  return _bzhi_u64(x, n);
#else
  return n >= 64 ? x : x & ((1ull << n) - 1);
#endif
}

// Count the number of 1s in the lowest n bits of x.
inline int popcount_low(uint64_t x, int n) { return port::popcount(low_bits(x, n)); }

// Returns the largest i such that x[i]=1 and 0<=i<n, or -1 if no such i exists.
inline int prev_set_bit(uint64_t x, int n) { return port::bit_width(low_bits(x, n)) - 1; }

// Returns the smallest i such that x[i]=1 and n<=i<64, or 64 if no such i exists.
inline int next_set_bit(uint64_t x, int n) { return n >= 64 ? 64 : port::countr_zero(x >> n << n); }

// Returns y where y[i]=x[i^xval]
inline uint64_t xor_permute(uint64_t x, int xor_val) {
  static constexpr uint64_t checkerboard[6]{0x5555555555555555ull, 0x3333333333333333ull, 0x0f0f0f0f0f0f0f0full,
                                            0x00ff00ff00ff00ffull, 0x0000ffff0000ffffull, 0x00000000ffffffffull};
  uint64_t ret = x;
  for (int i = 0; i < 6 && xor_val != 0; i++) {
    if (xor_val & 1) {
      uint64_t lo = ret & checkerboard[i];
      uint64_t hi = ret & ~checkerboard[i];
      lo <<= (1 << i);
      hi >>= (1 << i);
      ret = lo | hi;
    }
    xor_val >>= 1;
  }
  return ret;
}

}  // namespace cplib::impl

#pragma GCC pop_options