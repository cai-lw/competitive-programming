#pragma once

#include <cstdint>
#include <type_traits>

#include "cplib/num/pow.hpp"

namespace cplib {

/**
 * \brief Modular integer modulo \f$N=2^{61}-1\f$, a Mersenne prime.
 * \ingroup num
 *
 * A large prime modulus with exceptionally fast multiplication, this is suitable for when the only requirement for
 * the modulus is being sufficiently large. The most common use case is possibly rolling hash.
 */
class ModInt2P61M1 {
 public:
  using mint = ModInt2P61M1;
  using int_type = uint64_t;
  using int_double_t = unsigned __int128;
  static constexpr int_type MOD = (int_type(1) << 61) - 1;

  constexpr ModInt2P61M1() : val_(0) {}

  /**
   * \brief Converts a plain integer to a Barrett modular integer.
   *
   * This constructor is marked `explicit` because the cost of conversion is non-trivial (requires one
   * Barrett reduction) and thus implicit conversion is discouraged.
   */
  template <typename T, std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>>* = nullptr>
  explicit ModInt2P61M1(T x) {
    auto r = x % std::make_signed_t<int_type>(MOD);
    if (r < 0) {
      r += MOD;
    }
    val_ = r;
  }

  /** \copydoc ModInt2P61M1(T) */
  template <typename T, std::enable_if_t<std::is_unsigned_v<T>>* = nullptr>
  explicit ModInt2P61M1(T x) {
    val_ = x % MOD;
  }

  /** \brief Converts back to a plain integer in the range \f$[0,N)\f$. */
  int_type val() const { return val_; }

  /**
   * \brief Returns a number that is the same for the same residue class modulo the modulus.
   *
   * This is the same as val(), unlike MontgomeryModInt.
   */
  int_type residue() const { return val_; }

  /** \brief Returns the modulus. */
  static constexpr int_type mod() { return MOD; }

  mint& operator++() {
    val_++;
    if (val_ == MOD) {
      val_ = 0;
    }
    return *this;
  }

  mint operator++(int) {
    mint ret = *this;
    ++(*this);
    return ret;
  }

  mint operator+() const { return *this; }

  mint operator+(const mint& rhs) const {
    int_type r = val_ + rhs.val_;
    return from_raw(r >= MOD ? r - MOD : r);
  }

  mint& operator+=(const mint& rhs) { return *this = *this + rhs; }

  mint& operator--() {
    if (val_ == 0) {
      val_ = MOD - 1;
    } else {
      val_--;
    }
    return *this;
  }

  mint operator--(int) {
    mint ret = *this;
    --(*this);
    return ret;
  }

  mint operator-() const { return from_raw(val_ == 0 ? 0 : MOD - val_); }

  mint operator-(const mint& rhs) const {
    int_type r = val_ + MOD - rhs.val_;
    return from_raw(r >= MOD ? r - MOD : r);
  }

  mint& operator-=(const mint& rhs) { return *this = *this - rhs; }

  mint operator*(const mint& rhs) const {
    int_double_t prod = int_double_t(val_) * rhs.val_;
    int_type r = (prod >> 61) + (prod & MOD);
    return from_raw(r >= MOD ? r - MOD : r);
  }

  mint& operator*=(const mint& rhs) { return *this = *this * rhs; }

  mint inv() const { return pow(*this, MOD - 2); }

  mint operator/(const mint& rhs) const { return *this * rhs.inv(); }

  mint& operator/=(const mint& rhs) { return *this *= rhs.inv(); }

  bool operator==(const mint& rhs) const { return residue() == rhs.residue(); }

  bool operator!=(const mint& rhs) const { return !(*this == rhs); }

 private:
  int_type val_;

  static constexpr mint from_raw(int_type x) {
    mint ret;
    ret.val_ = x;
    return ret;
  }
};

}  // namespace cplib