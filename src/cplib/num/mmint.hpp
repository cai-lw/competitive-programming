#include <cstdint>
#include <limits>
#include <type_traits>
#include "cplib/num/gcd.hpp"
#include "cplib/utils/type.hpp"

namespace cplib {

/**
 * \brief Modular integer stored in Montgomery form.
 * \ingroup num
 * 
 * Your code should use the type alias ::MMInt or ::MMInt64 instead of this class.
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
 * \tparam UInt The unsigned integer type to store the number.
 * \tparam Mod The modulus. Must be odd and no larger than \f$1/4\f$ of the maximum value of UInt.
 */
template<typename UInt, UInt Mod, std::enable_if_t<std::is_unsigned_v<UInt>>* = nullptr>
class MontgomeryModInt {
public:
    static_assert(Mod % 2 == 1 && Mod <= std::numeric_limits<UInt>::max() / 4);
    using mint = MontgomeryModInt;
    using int_type = UInt;
    static constexpr int_type mod = Mod;

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
        signed_int_type v = x % signed_int_type(mod);
        _val = reduce(base2_mod * int_double_t(v < 0 ? v + mod : v));
    }

    /** \copydoc MontgomeryModInt(T) */
    template<typename T, std::enable_if_t<std::is_unsigned_v<T>>* = nullptr>
    explicit MontgomeryModInt(T x) {
        _val = reduce(base2_mod * int_double_t(x % mod));
    }
    
    /** \brief Converts back to a plain integer in the range \f$[0,N)\f$. */
    int_type val() const {
        return strict_shrink(reduce(_val));
    }

    mint& operator++() {
        _val = shrink(_val + base_mod);
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
        _val = shrink(_val + rhs._val);
        return *this;
    }

    mint operator+(const mint &rhs) const {
        mint ret = *this;
        ret += rhs;
        return ret;
    }

    mint& operator--() {
        _val = shrink(_val + (mod - base_mod));
        return *this;
    }

    mint operator--(int) {
        mint ret = *this;
        --(*this);
        return ret;
    }

    mint operator-() {
        return from_raw(_val == 0 ? 0 : mod * 2 - _val);
    }

    mint& operator-=(const mint &rhs) {
        _val = shrink(_val + (mod * 2 - rhs._val));
        return *this;
    }

    mint operator-(const mint &rhs) const {
        mint ret = *this;
        ret -= rhs;
        return ret;
    }

    mint& operator*=(const mint &rhs) {
        _val = reduce(int_double_t(_val) * rhs._val);
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
        return from_raw(reduce(int_double_t(base3_mod) * mod_inverse(_val, mod)));
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
        return strict_shrink(_val) == strict_shrink(rhs._val);
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

    // Modular inverse modulo 2^2^k by Hensel lifting.
    static constexpr int_type inv_base(int_type x) {
        int_type y = 1;
        for (int i = 1; i < base_width; i *= 2) {
            y *= int_type(2) - x * y;
        }
        return y;
    }

    using int_double_t = typename impl::make_double_width_t<int_type>;
    static constexpr int base_width = std::numeric_limits<int_type>::digits;
    static constexpr int_type mod_neg_inv = inv_base(~(mod - 1));
    static constexpr int_type base_mod = (int_double_t(1) << base_width) % mod;
    static constexpr int_type base2_mod = int_double_t(base_mod) * base_mod % mod;
    static constexpr int_type base3_mod = int_double_t(base2_mod) * base_mod % mod;

    static constexpr int_type shrink(int_type x) {
        return x >= mod * 2 ? x - mod * 2 : x;
    }

    static constexpr int_type strict_shrink(int_type x) {
        return x >= mod ? x - mod : x;
    }

    // Montgomery reduction without the final reduction step.
    static constexpr int_type reduce(int_double_t t) {
        int_type m = int_type(t) * mod_neg_inv;
        int_type r = (t + int_double_t(m) * mod) >> base_width;
        return r;
    }
};

/**
 * \brief Type alias for 32-bit MontgomeryModInt
 * \related MontgomeryModInt
 * \tparam Mod The modulus. Must be odd and no larger than \f$2^{30}-1\f$.
 */
template<uint32_t Mod>
using MMInt = MontgomeryModInt<uint32_t, Mod>;

/**
 * \brief Type alias for 64-bit MontgomeryModInt
 * \related MontgomeryModInt
 * \tparam Mod The modulus. Must be odd and no larger than \f$2^{62}-1\f$.
 */
template<uint64_t Mod>
using MMInt64 = MontgomeryModInt<uint64_t, Mod>;

}  // namespace cplib