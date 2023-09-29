#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

#include "cplib/num/gcd.hpp"
#include "cplib/port/bit.hpp"
#include "cplib/utils/type.hpp"

namespace cplib {

namespace impl {

template <typename UInt>
constexpr UInt barrett_mulh(UInt a, UInt b) {
  return make_double_width_t<UInt>(a) * b >> std::numeric_limits<UInt>::digits;
}

template <>
inline constexpr unsigned __int128 barrett_mulh(unsigned __int128 a, unsigned __int128 b) {
  uint64_t ah = a >> 64, al = a << 64 >> 64, bh = b >> 64, bl = b << 64 >> 64;
  using u128 = unsigned __int128;
  u128 l = u128(al) * bl;
  u128 h = u128(ah) * bh;
  u128 m = u128(al) * bh + u128(bl) * ah;
  h += m >> 64;
  return l + (m << 64) < l ? h + 1 : h;
}

template <typename UInt>
class BarrettReduction {
 public:
  using int_type = UInt;
  using int_double_t = make_double_width_t<int_type>;

  constexpr explicit BarrettReduction(int_type mod)
      : mod_(mod), red_(int_double_t(-1) / mod + port::has_single_bit(mod)) {}

  constexpr int_type mod() const { return mod_; }

  constexpr int_type mul(int_type a, int_type b) const {
    int_double_t m = int_double_t(a) * b;
    int_double_t r = m - barrett_mulh(m, red_) * mod_;
    return r >= mod_ ? r - mod_ : r;
  }

  constexpr int_type add(int_type a, int_type b) const {
    int_type r = a - (mod_ - b);
    return r > a ? r + mod_ : r;
  }

  constexpr int_type sub(int_type a, int_type b) const {
    int_type r = a - b;
    return r > a ? r + mod_ : r;
  }

 private:
  int_type mod_;
  int_double_t red_;
};

template <typename UInt, UInt Mod, std::enable_if_t<std::is_unsigned_v<UInt>>* = nullptr>
class StaticBarrettReductionContext {
 public:
  using int_type = UInt;
  using br_type = BarrettReduction<int_type>;
  static_assert(Mod > 1);

  static constexpr const br_type& barrett_reduction() { return reduction_; }

 private:
  static constexpr auto reduction_ = br_type(Mod);
};

template <typename UInt, std::enable_if_t<std::is_unsigned_v<UInt>>* = nullptr>
class DynamicBarrettReductionContext {
 public:
  using int_type = UInt;
  using br_type = BarrettReduction<int_type>;

  static constexpr const br_type& barrett_reduction() { return reduction_env_.back(); }

  static void push_mod(int_type mod) {
    assert(mod > 1);
    reduction_env_.emplace_back(mod);
  }

  static void pop_mod() { reduction_env_.pop_back(); }

 private:
  static inline std::vector<br_type> reduction_env_;
};

}  // namespace impl

/**
 * \brief Modular integer using Barrett reduction.
 * \ingroup num
 *
 * Your code should generally use the type alias ::BMInt or ::BMInt64 for compile-time static modulus, and
 * ::DynamicBMInt or ::DynamicBMInt64 for run-time dynamic modulus.
 *
 * Barrett reduction is used for modular multiplication to avoid costly division. Unlike Montgomery reduction, it works
 * for any modulus, but is slightly slower.
 *
 * \sa MontgomeryModInt
 */
template <typename Context>
class BarrettModInt {
  struct Guard;

 public:
  using mint = BarrettModInt;
  using int_type = typename Context::int_type;
  using br_type = typename Context::br_type;
  using int_double_t = typename br_type::int_double_t;

  BarrettModInt() : val_(0) {}

  /**
   * \brief Converts a plain integer to a Barrett modular integer.
   *
   * This constructor is marked `explicit` because the cost of conversion is non-trivial (requires one
   * Barrett reduction) and thus implicit conversion is discouraged.
   */
  template <typename T, std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>>* = nullptr>
  explicit BarrettModInt(T x) {
    auto r = x % impl::make_double_width_t<std::make_signed_t<int_type>>(br().mod());
    if (r < 0) {
      r += br().mod();
    }
    val_ = r;
  }

  /** \copydoc BarrettModInt(T) */
  template <typename T, std::enable_if_t<std::is_unsigned_v<T>>* = nullptr>
  explicit BarrettModInt(T x) {
    val_ = x % br().mod();
  }

  /**
   * \brief Set the dynamic modint's modulus.
   *
   * Calling this for static modint would be a compile error.
   *
   * It maintains a stack of moduli. Push stack when it is called, and pop stack when the returned guard object is
   * destructed. This allows recursively calling functions that use different moduli. However at any given moment
   * you can only use one modulus.
   *
   * \param mod Must be 2 or greater.
   */
  [[nodiscard]] static Guard set_mod_guard(int_type mod) {
    Context::push_mod(mod);
    return Guard();
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
  static constexpr int_type mod() { return br().mod(); }

  mint& operator++() {
    val_++;
    if (val_ == br().mod()) {
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

  mint operator+(const mint& rhs) const { return from_raw(br().add(val_, rhs.val_)); }

  mint& operator+=(const mint& rhs) { return *this = *this + rhs; }

  mint& operator--() {
    if (val_ == 0) {
      val_ = br().mod() - 1;
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

  mint operator-() const { return from_raw(br().sub(0, val_)); }

  mint operator-(const mint& rhs) const { return from_raw(br().sub(val_, rhs.val_)); }

  mint& operator-=(const mint& rhs) { return *this = *this - rhs; }

  mint operator*(const mint& rhs) const { return from_raw(br().mul(val_, rhs.val_)); }

  mint& operator*=(const mint& rhs) { return *this = *this * rhs; }

  /**
   * \brief Returns the modular inverse.
   *
   * The underlying value must be non-zero.
   */
  mint inv() const { return from_raw(mod_inverse(val_, br().mod())); }

  mint operator/(const mint& rhs) const { return *this * rhs.inv(); }

  mint& operator/=(const mint& rhs) { return *this *= rhs.inv(); }

  bool operator==(const mint& rhs) const { return residue() == rhs.residue(); }

  bool operator!=(const mint& rhs) const { return !(*this == rhs); }

 private:
  int_type val_;

  struct Guard {
    ~Guard() { Context::pop_mod(); }
  };

  static constexpr mint from_raw(int_type x) {
    mint ret;
    ret.val_ = x;
    return ret;
  }

  static constexpr const br_type& br() { return Context::barrett_reduction(); }
};

/**
 * \brief Type alias for 32-bit BarrettModInt with compile-time constant modulus.
 * \related BarrettModInt
 * \tparam Mod The modulus. Must be within \f$[2,2^{32})\f$.
 */
template <uint32_t Mod>
using BMInt = BarrettModInt<impl::StaticBarrettReductionContext<uint32_t, Mod>>;

/**
 * \brief Type alias for 64-bit BarrettModInt with compile-time constant modulus.
 * \related BarrettModInt
 * \tparam Mod The modulus. Must be within \f$[2,2^{64})\f$.
 */
template <uint64_t Mod>
using BMInt64 = BarrettModInt<impl::StaticBarrettReductionContext<uint64_t, Mod>>;

/**
 * \brief Type alias for dynamic BarrettModInt with modulus less than \f$2^{32}\f$.
 * \related BarrettModInt
 */
using DynamicBMInt = BarrettModInt<impl::DynamicBarrettReductionContext<uint32_t>>;

/**
 * \brief Type alias for dynamic BarrettModInt with modulus less than \f$2^{64}\f$.
 * \related BarrettModInt
 */
using DynamicBMInt64 = BarrettModInt<impl::DynamicBarrettReductionContext<uint64_t>>;

}  // namespace cplib