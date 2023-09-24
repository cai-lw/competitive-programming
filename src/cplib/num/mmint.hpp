#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

#include "cplib/num/gcd.hpp"
#include "cplib/utils/type.hpp"

namespace cplib {

namespace impl {

template <typename UInt>
class MontgomeryReductionBase {
 public:
  using int_type = UInt;
  using int_double_t = make_double_width_t<int_type>;

  constexpr explicit MontgomeryReductionBase(int_type mod)
      : mod_(mod),
        mod_neg_inv_(inv_base(-mod)),
        mbase_((int_double_t(1) << base_width_) % mod),
        mbase2_(int_double_t(mbase_) * mbase_ % mod),
        mbase3_(int_double_t(mbase2_) * mbase_ % mod) {}

  // N
  constexpr int_type mod() const { return mod_; }

  // R%N
  constexpr int_type mbase() const { return mbase_; }

  // R^2%N
  constexpr int_type mbase2() const { return mbase2_; }

  // R^3%N
  constexpr int_type mbase3() const { return mbase3_; }

 protected:
  int_type mod_, mod_neg_inv_, mbase_, mbase2_, mbase3_;
  static constexpr int base_width_ = std::numeric_limits<int_type>::digits;

 private:
  // Modular inverse modulo 2^2^k by Hensel lifting.
  static constexpr int_type inv_base(int_type x) {
    int_type y = 1;
    for (int i = 1; i < base_width_; i *= 2) {
      y *= int_type(2) - x * y;
    }
    return y;
  }
};

// Value in [0,2N), only works if N<R/4
template <typename UInt>
class MontgomeryReductionLoose : public MontgomeryReductionBase<UInt> {
 public:
  using Base = MontgomeryReductionBase<UInt>;
  using Base::Base;
  using typename Base::int_double_t;
  using typename Base::int_type;

  // a*b*(R^-1)%N. Result <2N if input <2N.
  constexpr int_type mul(int_type a, int_type b) const {
    int_double_t t = int_double_t(a) * b;
    int_type m = int_type(t) * this->mod_neg_inv_;
    int_type r = (t + int_double_t(m) * this->mod_) >> this->base_width_;
    return r;
  }

  // (a+b)%N. Result <2N if input <2N.
  constexpr int_type add(int_type a, int_type b) const {
    int_type r = a + b;
    return r >= this->mod_ * 2 ? r - this->mod_ * 2 : r;
  }

  // (a-b)%N. Result <2N if input <2N.
  constexpr int_type sub(int_type a, int_type b) const {
    int_type r = a - b;
    return r > a ? r + this->mod_ * 2 : r;
  }

  // Shrink value from [0,2N) into [0,N)
  constexpr int_type shrink(int_type x) const { return x >= this->mod_ ? x - this->mod_ : x; }
};

// Value in [0,N), works for all N<R
template <typename UInt>
class MontgomeryReductionStrict : public MontgomeryReductionBase<UInt> {
 public:
  using Base = MontgomeryReductionBase<UInt>;
  using Base::Base;
  using typename Base::int_double_t;
  using typename Base::int_type;

  // We use the same technique for the following functions where the result R is in [0,2N) where 2N may overflow.
  // If the last step is R=X+Y where X,Y are in [0,N), we make it R'=X-(N-Y) so that the result is in [-N,N)
  // The "true" value of R' is negative iff the last subtraction underflows, iff R'>X, and that's exactly when we
  // add N to R' to bring the value back to [0,N).

  // a*b*(R^-1)%N
  constexpr int_type mul(int_type a, int_type b) const {
    int_double_t t = int_double_t(a) * b;
    int_type m = int_type(t) * this->mod_neg_inv_;
    int_double_t s = t - int_double_t(-m) * this->mod_;
    int_type r = s >> this->base_width_;
    return s > t ? r + this->mod_ : r;
  }

  // (a+b)%N
  constexpr int_type add(int_type a, int_type b) const {
    int_type r = a - (this->mod_ - b);
    return r > a ? r + this->mod_ : r;
  }

  // (a-b)%N
  constexpr int_type sub(int_type a, int_type b) const {
    int_type r = a - b;
    return r > a ? r + this->mod_ : r;
  }

  // No-op
  constexpr int_type shrink(int_type x) const { return x; }
};

template <typename UInt, UInt Mod, std::enable_if_t<std::is_unsigned_v<UInt>>* = nullptr>
class StaticMontgomeryReductionContext {
 public:
  using int_type = UInt;
  using mr_type =
      std::conditional_t<Mod <= std::numeric_limits<int_type>::max() / 4, impl::MontgomeryReductionLoose<int_type>,
                         impl::MontgomeryReductionStrict<int_type>>;
  static_assert(Mod % 2 == 1);

  static constexpr const mr_type& montgomery_reduction() { return reduction_; }

 private:
  static constexpr auto reduction_ = mr_type(Mod);
};

template <typename UInt, bool Loose, std::enable_if_t<std::is_unsigned_v<UInt>>* = nullptr>
class DynamicMontgomeryReductionContext {
 public:
  using int_type = UInt;
  using mr_type =
      std::conditional_t<Loose, impl::MontgomeryReductionLoose<int_type>, impl::MontgomeryReductionStrict<int_type>>;

  static constexpr const mr_type& montgomery_reduction() { return reduction_env_.back(); }

  static void push_mod(int_type mod) {
    assert(mod % 2 == 1);
    if constexpr (Loose) {
      assert(mod <= std::numeric_limits<int_type>::max() / 4);
    }
    reduction_env_.emplace_back(mod);
  }

  static void pop_mod() { reduction_env_.pop_back(); }

 private:
  static inline std::vector<mr_type> reduction_env_;
};

}  // namespace impl

/**
 * \brief Modular integer stored in Montgomery form.
 * \ingroup num
 *
 * Your code should generally use the type alias ::MMInt or ::MMInt64 for compile-time static modulus, or one of
 * ::DynamicMMInt30, ::DynamicMMInt32, ::DynamicMMInt62, ::DynamicMMInt64 for runtime dynamic modulus.
 *
 * Unless converting between modular integers and ordinary integers very frequently (which is rarely the case),
 * Montgomery modular integer is preferred over plain modular integer (such as `atcoder::modint`).
 *
 * For modulus with \f$n\f$ bits, Montgomery reduction uses multiplication result of up to \f$2n\f$ bits, whereas
 * Barrett reduction (for modular multiplication of numbers stored in plain form) uses up to \f$3n\f$ bits.
 * Thus, for 32-bit modulus Barrett reduction is less SIMD-friendly due to requiring 128-bit multiplication,
 * and for 64-bit modulus Barrett reduction is significantly slower due to requiring multi-precision multiplication.
 *
 * When \f$N<R/4\f$, where \f$N\f$ is the modulus and \f$R=2^{32}\f$ or \f$2^{64}\f$ the Montgomery divisor,
 * this implementation makes further optimization to reduce branching and improve SIMD-friendliness.
 * We keep everything in \f$[0,2N)\f$ instead of \f$[0,N)\f$. The result of multiplication-Montgomery reduction of
 * two numbers less than \f$2N\f$, even without the final reduction step, is already less than
 * \f$((2N)(2N)+NR)/R=N(4N/R)+N<2N\f$, thus the final reduction step is not needed.
 * 
 * \sa BarrettModInt
 */
template <typename Context>
class MontgomeryModInt {
  struct Guard;

 public:
  using mint = MontgomeryModInt;
  using int_type = typename Context::int_type;
  using mr_type = typename Context::mr_type;
  using int_double_t = typename mr_type::int_double_t;

  MontgomeryModInt() : val_(0) {}

  /**
   * \brief Converts a plain integer to a Montgomery modular integer.
   *
   * This constructor is marked `explicit` because the cost of conversion is non-trivial (requires one
   * Montgomery reduction) and thus implicit conversion is discouraged.
   */
  template <typename T, std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>>* = nullptr>
  explicit MontgomeryModInt(T x) {
    auto r = x % impl::make_double_width_t<std::make_signed_t<int_type>>(mr().mod());
    if (r < 0) {
      r += mr().mod();
    }
    val_ = mr().mul(mr().mbase2(), r);
  }

  /** \copydoc MontgomeryModInt(T) */
  template <typename T, std::enable_if_t<std::is_unsigned_v<T>>* = nullptr>
  explicit MontgomeryModInt(T x) {
    val_ = mr().mul(mr().mbase2(), x % mr().mod());
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
   * \param mod Must be odd.
   */
  [[nodiscard]] static Guard set_mod_guard(int_type mod) {
    Context::push_mod(mod);
    return Guard();
  }

  /** \brief Converts back to a plain integer in the range \f$[0,N)\f$. */
  int_type val() const { return mr().shrink(mr().mul(1, val_)); }

  /**
   * \brief Returns a number that is the same for the same residue class modulo the modulus.
   *
   * This is faster than val(), but the number is not the remainder. Useful as key in associative containers.
   */
  int_type residue() const { return mr().shrink(val_); }

  /** \brief Returns the modulus. */
  static constexpr int_type mod() { return mr().mod(); }

  mint& operator++() {
    val_ = mr().add(val_, mr().mbase());
    return *this;
  }

  mint operator++(int) {
    mint ret = *this;
    ++(*this);
    return ret;
  }

  mint operator+() const { return *this; }

  mint operator+(const mint& rhs) const { return from_raw(mr().add(val_, rhs.val_)); }

  mint& operator+=(const mint& rhs) { return *this = *this + rhs; }

  mint& operator--() {
    val_ = mr().sub(val_, mr().mbase());
    return *this;
  }

  mint operator--(int) {
    mint ret = *this;
    --(*this);
    return ret;
  }

  mint operator-() const { return from_raw(mr().sub(0, val_)); }

  mint operator-(const mint& rhs) const { return from_raw(mr().sub(val_, rhs.val_)); }

  mint& operator-=(const mint& rhs) { return *this = *this - rhs; }

  mint operator*(const mint& rhs) const { return from_raw(mr().mul(val_, rhs.val_)); }

  mint& operator*=(const mint& rhs) { return *this = *this * rhs; }

  /**
   * \brief Returns the modular inverse.
   *
   * Requires the underlying value to be invertible, i.e. coprime with the modulus.
   */
  mint inv() const { return from_raw(mr().mul(mr().mbase3(), mod_inverse(val_, mr().mod()))); }

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

  static constexpr const mr_type& mr() { return Context::montgomery_reduction(); }
};

/**
 * \brief Type alias for 32-bit MontgomeryModInt with compile-time constant modulus.
 * \related MontgomeryModInt
 * \tparam Mod The modulus. Must be odd and less than \f$2^{32}\f$.
 */
template <uint32_t Mod>
using MMInt = MontgomeryModInt<impl::StaticMontgomeryReductionContext<uint32_t, Mod>>;

/**
 * \brief Type alias for 64-bit MontgomeryModInt with compile-time constant modulus.
 * \related MontgomeryModInt
 * \tparam Mod The modulus. Must be odd and less than \f$2^{64}\f$.
 */
template <uint64_t Mod>
using MMInt64 = MontgomeryModInt<impl::StaticMontgomeryReductionContext<uint64_t, Mod>>;

/**
 * \brief Type alias for dynamic MontgomeryModInt with modulus less than \f$2^{30}\f$.
 * \related MontgomeryModInt
 */
using DynamicMMInt30 = MontgomeryModInt<impl::DynamicMontgomeryReductionContext<uint32_t, true>>;

/**
 * \brief Type alias for dynamic MontgomeryModInt with modulus less than \f$2^{32}\f$.
 * \related MontgomeryModInt
 */
using DynamicMMInt32 = MontgomeryModInt<impl::DynamicMontgomeryReductionContext<uint32_t, false>>;

/**
 * \brief Type alias for dynamic MontgomeryModInt with modulus less than \f$2^{62}\f$.
 * \related MontgomeryModInt
 */
using DynamicMMInt62 = MontgomeryModInt<impl::DynamicMontgomeryReductionContext<uint64_t, true>>;

/**
 * \brief Type alias for dynamic MontgomeryModInt with modulus less than \f$2^{64}\f$.
 * \related MontgomeryModInt
 */
using DynamicMMInt64 = MontgomeryModInt<impl::DynamicMontgomeryReductionContext<uint64_t, false>>;

/**
 * \brief Given a modulus, calls a callable (visitor) with a dynamically selected fastest MontgomeryModInt type.
 *
 * The visitor is called like `visitor(mint(x))` where `mint` is the fastest dynamic MontgomeryModInt type that can
 * handle `mod` as the modulus, and its modulus set to `mod`. The visitor should be able to accept different dynamic
 * MontgomeryModInt types, similar to the visitor for std::visit.
 *
 * \related MontgomeryModInt
 */
template <typename Visitor, typename UInt, typename... Args>
auto mmint_by_modulus(Visitor&& visitor, UInt mod, Args&&... args) {
  if (mod <= std::numeric_limits<UInt>::max() / 4) {
    using mint = MontgomeryModInt<impl::DynamicMontgomeryReductionContext<UInt, true>>;
    auto _guard = mint::set_mod_guard(mod);
    if constexpr (sizeof...(args) == 0) {
      return visitor(mint());
    } else {
      return visitor(mint(args)...);
    }
  } else {
    using mint = MontgomeryModInt<impl::DynamicMontgomeryReductionContext<UInt, false>>;
    auto _guard = mint::set_mod_guard(mod);
    if constexpr (sizeof...(args) == 0) {
      return visitor(mint());
    } else {
      return visitor(mint(args)...);
    }
  }
};

}  // namespace cplib