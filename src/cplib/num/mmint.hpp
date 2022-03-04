#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>
#include "cplib/num/gcd.hpp"
#include "cplib/utils/type.hpp"

namespace cplib {

namespace impl {

template<typename UInt>
class MontgomeryReduction {
public:
    using int_type = UInt;
    using int_double_t = make_double_width_t<int_type>;
    static constexpr int base_width = std::numeric_limits<int_type>::digits;

    constexpr explicit MontgomeryReduction(int_type mod) :
        _mod(mod),
        _mod_neg_inv(inv_base(~(mod - 1))),
        _mbase((int_double_t(1) << base_width) % mod),
        _mbase2(int_double_t(_mbase) * _mbase % mod),
        _mbase3(int_double_t(_mbase2) * _mbase % mod)
        {}

    // N
    constexpr int_type mod() const { return _mod; }

    // R%N
    constexpr int_type mbase() const { return _mbase; }

    // R^2%N
    constexpr int_type mbase2() const { return _mbase2; }

    // R^3%N
    constexpr int_type mbase3() const { return _mbase3; }

    // T*(R^-1)%N. Result <2N if input <2N*2N.
    constexpr int_type reduce(int_double_t t) const {
        int_type m = int_type(t) * _mod_neg_inv;
        int_type r = (t + int_double_t(m) * _mod) >> base_width;
        return r;
    }

    // Shrink value into range [0,2N)
    constexpr int_type shrink(int_type x) const {
        return x >= _mod * 2 ? x - _mod * 2 : x;
    }

    // Shrink value into range [0,N)
    constexpr int_type strict_shrink(int_type x) const {
        return x >= _mod ? x - _mod : x;
    }

private:
    int_type _mod, _mod_neg_inv, _mbase, _mbase2, _mbase3;

    // Modular inverse modulo 2^2^k by Hensel lifting.
    static constexpr int_type inv_base(int_type x) {
        int_type y = 1;
        for (int i = 1; i < base_width; i *= 2) {
            y *= int_type(2) - x * y;
        }
        return y;
    }
};

}  // namespace impl

/**
 * \brief Modular integer stored in Montgomery form.
 * \ingroup num
 * 
 * For static modint, your code should generally use the type alias ::MMInt or ::MMInt64. For dynamic modint, see
 * DynamicMontgomeryReductionContext for example.
 * 
 * Unless converting between modular integers and ordinary integers very frequently (which is rarely the case),
 * Montgomery modular integer is preferred over plain modular integer (such as `atcoder::modint`).
 * 
 * For modulus with \f$n\f$ bits, Montgomery reduction uses multiplication result of up to \f$2n\f$ bits, whereas
 * Barrett reduction (for modular multiplication of numbers stored in plain form) uses up to \f$3n\f$ bits.
 * Thus, for 32-bit modulus Barrett reduction is less SIMD-friendly due to requiring 128-bit multiplication,
 * and for 64-bit modulus Barrett reduction is significantly slower due to requiring multi-precision multiplication.
 * 
 * This implementation makes further optimization to reduce branching and improve SIMD-friendliness, at the cost of
 * requiring \f$N<R/4\f$, where \f$N\f$ is the modulus and \f$R=2^{32}\f$ or \f$2^{64}\f$ the Montgomery divisor.
 * We keep everything in \f$[0,2N)\f$ instead of \f$[0,N)\f$. The result of multiplication-Montgomery reduction of
 * two numbers less than \f$2N\f$, even without the final reduction step, is already less than
 * \f$((2N)(2N)+NR)/R=N(4N/R)+N<2N\f$, thus the final reduction step is not needed.
 * 
 * \tparam Context ::StaticMontgomeryReductionContext or ::DynamicMontgomeryReductionContext. Provides information
 * for performing Montgomery reduction.
 */
template<typename Context>
class MontgomeryModInt {
public:
    using mint = MontgomeryModInt;
    using int_type = typename Context::int_type;
    using mr_type = typename Context::mr_type;
    using int_double_t = typename mr_type::int_double_t;

    MontgomeryModInt() : _val(0) {}

    /**
     * \brief Converts a plain integer to a Montgomery modular integer.
     * 
     * This constructor is marked `explicit` because the cost of conversion is non-trivial (requires one
     * Montgomery reduction) and thus implicit conversion is discouraged.
     */
    template<typename T, std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>>* = nullptr>
    explicit MontgomeryModInt(T x) {
        using signed_int_type = std::make_signed_t<int_type>;
        signed_int_type v = x % signed_int_type(mr().mod());
        _val = mr().reduce(mr().mbase2() * int_double_t(v < 0 ? v + mr().mod() : v));
    }

    /** \copydoc MontgomeryModInt(T) */
    template<typename T, std::enable_if_t<std::is_unsigned_v<T>>* = nullptr>
    explicit MontgomeryModInt(T x) {
        _val = mr().reduce(mr().mbase2() * int_double_t(x % mr().mod()));
    }
    
    /** \brief Converts back to a plain integer in the range \f$[0,N)\f$. */
    int_type val() const {
        return mr().strict_shrink(mr().reduce(_val));
    }

    /** \brief Returns the modulus. */
    static constexpr int_type mod() { return mr().mod(); }

    mint& operator++() {
        _val = mr().shrink(_val + mr().mbase());
        return *this;
    }

    mint operator++(int) {
        mint ret = *this;
        ++(*this);
        return ret;
    }

    mint operator+() {
        return *this;
    }

    mint& operator+=(const mint &rhs) {
        _val = mr().shrink(_val + rhs._val);
        return *this;
    }

    mint operator+(const mint &rhs) const {
        mint ret = *this;
        ret += rhs;
        return ret;
    }

    mint& operator--() {
        _val = mr().shrink(_val + mr().mod() - mr().mbase());
        return *this;
    }

    mint operator--(int) {
        mint ret = *this;
        --(*this);
        return ret;
    }

    mint operator-() {
        return from_raw(_val == 0 ? 0 : mr().mod() * 2 - _val);
    }

    mint& operator-=(const mint &rhs) {
        _val = mr().shrink(_val + mr().mod() * 2 - rhs._val);
        return *this;
    }

    mint operator-(const mint &rhs) const {
        mint ret = *this;
        ret -= rhs;
        return ret;
    }

    mint& operator*=(const mint &rhs) {
        _val = mr().reduce(int_double_t(_val) * rhs._val);
        return *this;
    }

    mint operator*(const mint &rhs) const {
        mint ret = *this;
        ret *= rhs;
        return ret;
    }

    /**
     * \brief Returns the modular inverse.
     * 
     * Requires the underlying value to be invertible, i.e. coprime with the modulus.
     */
    mint inv() const {
        return from_raw(mr().reduce(int_double_t(mr().mbase3()) * mod_inverse(_val, mr().mod())));
    }

    mint& operator/=(const mint &rhs) {
        return *this *= rhs.inv();
    }

    mint operator/(const mint &rhs) const {
        mint ret = *this;
        ret /= rhs;
        return ret;
    }

    bool operator==(const mint &rhs) const {
        return mr().strict_shrink(_val) == mr().strict_shrink(rhs._val);
    }

    bool operator!=(const mint &rhs) const {
        return !(*this == rhs);
    }

private:
    int_type _val;

    static constexpr mint from_raw(int_type x) {
        mint ret;
        ret._val = x;
        return ret;
    }

    static constexpr const mr_type& mr() {
        return Context::montgomery_reduction();
    }
};

/**
 * \brief Compile-time constant modulus for Montgomery reduction.
 * \ingroup num
 * 
 * All necessary information is computed at compile-time.
 * 
 * \tparam UInt An unsigned integer type.
 * \tparam Mod The modulus. Must be odd and no larger than than 1/4 of `UInt`'s maximum value.
 * \see MontgomeryModInt
 */
template<typename UInt, UInt Mod, std::enable_if_t<std::is_unsigned_v<UInt>>* = nullptr>
class StaticMontgomeryReductionContext {
public:
    using int_type = UInt;
    using mr_type = impl::MontgomeryReduction<int_type>;
    static_assert(Mod % 2 == 1 && Mod <= std::numeric_limits<int_type>::max() / 4);

    static constexpr const mr_type& montgomery_reduction() {
        return _reduction;
    }

private:
    static constexpr auto _reduction = mr_type(Mod);
};

/**
 * \brief Runtime mutable modulus for Montgomery reduction.
 * \ingroup num
 * 
 * Maintains a stack of moduli. Push stack when set_mod is called, and pop stack when the ModGuard object goes
 * out of scope. This allows recursively calling functions that use different moduli. However at any given moment
 * you can only use one modulus.
 * 
 * Creating a dynamic MontgomeryModInt under a modulus and using it under another is undefined behavior.
 * 
 * Below is an example of using this class for dynamic MontgomeryModInt.
 * 
 * ```
 * uint32_t do_something(uint32_t mod) {
 *     using ctx = DynamicMontgomeryReductionContext<uint32_t>;
 *     auto _guard = ctx::set_mod(mod);
 *     // Now the new modulus is pushed and `_guard` is alive.
 *     using mint = MontgomeryModInt<ctx>;
 *     mint ans(42);
 *     // Do some modular arithmetic here using the new modulus. It's okay to call functions that may use differnt
 *     // moduli, since their moduli will be popped after they finish.
 *     return ans.val();
 *     // `_guard` is destructed and the modulus is popped.
 * }
 * ```
 * 
 * \tparam UInt An unsigned integer type.
 */
template<typename UInt, std::enable_if_t<std::is_unsigned_v<UInt>>* = nullptr>
class DynamicMontgomeryReductionContext {
public:
    using int_type = UInt;
    using mr_type = impl::MontgomeryReduction<int_type>;

    struct ModGuard {
        ModGuard(const ModGuard&) = delete;
        ModGuard(ModGuard&&) = delete;
        ModGuard& operator=(const ModGuard&) = delete;
        ModGuard& operator=(ModGuard&&) = delete;
        ~ModGuard() {
            _reduction_env.pop_back();
        }
    private:
        friend DynamicMontgomeryReductionContext;
        ModGuard() {};
    };

    /**
     * \brief Set the modulus.
     * 
     * The returned ModGuard object must stay alive when computing under this modulus.
     * 
     * \param mod Must be odd and no larger than than 1/4 of `UInt`'s maximum value.
     */
    [[nodiscard]]
    static ModGuard set_mod(int_type mod) {
        assert(mod % 2 == 1 && mod <= std::numeric_limits<int_type>::max() / 4);
        _reduction_env.emplace_back(mod);
        return ModGuard();
    }

    static constexpr const mr_type& montgomery_reduction() {
        return _reduction_env.back();
    }

private:
    static inline std::vector<mr_type> _reduction_env;
};

/**
 * \brief Type alias for 32-bit MontgomeryModInt with compile-time constant modulus.
 * \related MontgomeryModInt
 * \tparam Mod The modulus. Must be odd and no larger than \f$2^{30}-1\f$.
 */
template<uint32_t Mod>
using MMInt = MontgomeryModInt<StaticMontgomeryReductionContext<uint32_t, Mod>>;

/**
 * \brief Type alias for 64-bit MontgomeryModInt with compile-time constant modulus.
 * \related MontgomeryModInt
 * \tparam Mod The modulus. Must be odd and no larger than \f$2^{62}-1\f$.
 */
template<uint64_t Mod>
using MMInt64 = MontgomeryModInt<StaticMontgomeryReductionContext<uint64_t, Mod>>;

}  // namespace cplib