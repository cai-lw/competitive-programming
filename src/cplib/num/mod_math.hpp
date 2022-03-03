#include <optional>
#include "cplib/num/mmint.hpp"

namespace cplib {

template<typename R>
R pow(R base, uint64_t exp) {
    R res(1);
    while (exp) {
        if (exp & 1) {
            res *= base;
        }
        base *= base;
        exp >>= 1;
    }
    return res;
}

/**
 * \brief Square root modulo a prime number.
 * \ingroup num
 * 
 * Returns a \f$x\f$ such that \f$x^2\equiv n \pmod{p}\f$, or `std::nullopt` if it doesn't exist. \f$-x\f$ is always
 * also a solution and there is no other solution.
 * 
 * This is implemented using [Cipolla's algorithm](https://en.wikipedia.org/wiki/Cipolla%27s_algorithm), as the common
 * alternative [Tonelli-Shanks algorithm](https://en.wikipedia.org/wiki/Tonelli%E2%80%93Shanks_algorithm) is slow
 * when \f$p-1\f$ is divisible by a high power of 2, which is commonly the case in competitive programming, since this
 * is exactly the requirement for a prime to be FFT-friendly.
 * 
 * \tparam Fp Finite field \f$\mathbb{F}_p\f$ where \f$p\f$ is prime.
 */
template<typename Fp>
std::optional<Fp> sqrt_mod_prime(Fp n){
    const Fp fp0(0), fp1(1);
    auto p = Fp::mod();
    if (n == fp0 || p == 2) {
        return n;
    } else if (pow(n, (p - 1) / 2) != fp1)
        return std::nullopt;
    else if (p % 4 == 3) {
        return pow(n, (p + 1) / 4);
    }
    // 
    Fp a, w2;
    do {
        a += fp1;
        w2 = a * a - n;
    } while(pow(w2, (p - 1) / 2) == fp1);
    std::pair<Fp, Fp> pow{a, fp1};
    std::pair<Fp, Fp> ret{fp1, fp0};
    auto e = (p + 1) / 2;
    while (e) {
        auto [ap, bp] = pow;
        if(e & 1) {
            auto [ar, br] = ret;
            // Save one multiplication using the Karatsuba technique
            Fp arap = ar * ap;
            Fp brbp = br * bp;
            Fp arbp_brap = (ar + br) * (ap + bp) - (arap + brbp);
            ret = {arap + brbp * w2, arbp_brap};
        }
        Fp apbp = ap * bp;
        pow = {ap * ap + bp * bp * w2, apbp + apbp};
        e >>= 1;
    }
    return ret.first;
}

/**
 * \copybrief sqrt_mod_prime(Fp)
 * \ingroup num
 * 
 * Returns a \f$x\f$ such that \f$x^2\equiv n \pmod{p}\f$ and \f$0\leq x<p\f$, or `std::nullopt` if it doesn't exist.
 * If \f$x\neq 0\f$ then \f$p-x\f$ is always also a solution, and there is no other solution.
 * 
 * \tparam T An unsigned integer type
 * \param p A prime number no larger than 1/4 the maximum value of `T`. This is the same requirement for the modulus of
 * MontgomeryModInt since it is used for all modular arithmetic in this function.
 */
template<typename T, std::enable_if_t<std::is_unsigned_v<T>>* = nullptr>
std::optional<T> sqrt_mod_prime(T n, T p) {
    if (p == 2) {
        // Cannot use MontgomeryModInt since 2 is even.
        return n % 2;
    }
    using ctx = DynamicMontgomeryReductionContext<T>;
    auto _guard = ctx::set_mod(p);
    using mint = MontgomeryModInt<ctx>;
    std::optional<mint> ret = sqrt_mod_prime(mint(n));
    if (ret) {
        return ret->val();
    } else {
        return std::nullopt;
    }
}

}  // namespace cplib