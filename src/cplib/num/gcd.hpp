#include <cassert>
#include <tuple>
#include <type_traits>
#include "cplib/port/bit.hpp"

namespace cplib {

/**
 * \brief Greatest common divisor.
 * \ingroup num
 * 
 * `std::gcd` is available since C++17, but the GCC implementaiton uses the slow Euclidean algorithm until version 11.
 * This implementation uses binary GCD which is generally several times faster.
 * 
 * Note that, unlike `std::gcd`, this function only accepts unsigned integers.
 */
template<typename T, std::enable_if_t<std::is_unsigned_v<T>>* = nullptr>
T gcd(T x, T y) {
    if (x == 0) {
        return y;
    } else if (y == 0) {
        return x;
    }
    int kx = port::countr_zero(x);
    int ky = port::countr_zero(y);
    x >>= kx;
    y >>= ky;
    while (y != 0) {
        y >>= port::countr_zero(y);
        if (x > y) {
            std::swap(x, y);
        }
        y -= x;
    }
    return x << std::min(kx, ky);
}

/**
 * \brief [Bézout coefficients](https://en.wikipedia.org/wiki/B%C3%A9zout%27s_identity), i.e. \f$(a,b)\f$ such that
 * \f$ax+by=\mathrm{GCD}(x,y)\f$.
 * \ingroup num
 * 
 * Returns a 3-tuple \f$(a,b,d)\f$ such that \f$ax+by=d\f$ where \f$d=\mathrm{GCD}(x,y)\f$. It is guaranteed that either
 * \f$|a|\leq\lfloor\frac{y}{2d}\rfloor, |b|\leq\lfloor\frac{x}{2d}\rfloor\f$ or \f$(a,b)\in\{(0,0),(0,1),(1,0)\}\f$.
 * In other words, \f$(a,b)\f$ is always the unique solution with both \f$|a|\f$ and \f$|b|\f$ being the smallest.
 * 
 * The above property shows that, for input `x` and `y` of an unsigned integer type `T`, the output `a` and `b` can 
 * always fit in the signed integer type with the same width as `T`, i.e. `std::make_signed_t<T>`.
 * Thus, `a` and `b` are returned as such type.
 * 
 * This is implemented using the extended Euclidean algorithm. Various extended binary GCD algorithms exist, but they
 * either cannot guarantee to find a unique or small solution, or are not faster than the extended Euclidean algorithm
 * due to extra bound checks needed for finding a unique or small solution.
 */
template<typename T, std::enable_if_t<std::is_unsigned_v<T>>* = nullptr>
std::tuple<std::make_signed_t<T>, std::make_signed_t<T>, T> bezout(T x, T y) {
    bool swap = x < y;
    if (swap) {
        std::swap(x, y);
    }
    if (y == 0) {
        if (x == 0) {
            return {0, 0, 0};
        } else if (swap) {
            return {0, 1, x};
        } else {
            return {1, 0, x};
        }
    }
    using S = std::make_signed_t<T>;
    S s0 = 1, s1 = 0, t0 = 0, t1 = 1;
    while (true) {
        T q = x / y, r = x % y;
        if (r == 0) {
            if (swap) {
                return {t1, s1, y};
            } else {
                return {s1, t1, y};
            }
        }
        S s2 = s0 - S(q) * s1, t2 = t0 - S(q) * t1;
        x = y;
        y = r;
        s0 = s1;
        s1 = s2;
        t0 = t1;
        t1 = t2;
    }
}

/**
 * \brief Modular inverse.
 * \ingroup num
 * 
 * Returns the unique \f$y\f$ such that \f$xy\equiv 1\pmod{m}\f$ and \f$0\leq y<m\f$.
 * 
 * Requires \f$\mathrm{GCD}(x,m)=1\f$. Note that \f$m\f$ does not have to be a prime.
 */
template<typename T, std::enable_if_t<std::is_unsigned_v<T>>* = nullptr>
T mod_inverse(T x, T m) {
    auto [s, t, g] = bezout(x, m);
    assert(g == 1);
    return s < 0 ? T(s) + m : s;
}

}  // namespace cplib