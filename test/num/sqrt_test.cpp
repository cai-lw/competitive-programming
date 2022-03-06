#include "catch2/catch.hpp"
#include "cplib/num/sqrt.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Modular sqaure root", "[sqrt]") {
    REQUIRE(*sqrt_mod_prime(0u, 2u) == 0u);
    REQUIRE(*sqrt_mod_prime(1u, 2u) == 1u);
    REQUIRE(!sqrt_mod_prime(3u, 7u));
    uint32_t sqrt2m7 = *sqrt_mod_prime(2u, 7u);
    REQUIRE((sqrt2m7 == 3u || sqrt2m7 == 4u));
    uint32_t sqrt11m7 = *sqrt_mod_prime(11u, 7u);
    REQUIRE((sqrt11m7 == 2u || sqrt11m7 == 5u));
    REQUIRE(*sqrt_mod_prime(0u, 998244353u) == 0u);
    uint32_t sqrt2m998244353 = *sqrt_mod_prime(2u, 998244353u);
    REQUIRE((sqrt2m998244353 == 116195171u || sqrt2m998244353 == 998244353u - 116195171u));

    using mint = MMInt<13>;
    REQUIRE(sqrt_mod_prime(mint(0))->val() == 0u);
    REQUIRE(!sqrt_mod_prime(mint(2)));
    uint32_t sqrt3m13 = sqrt_mod_prime(mint(3))->val();
    REQUIRE((sqrt3m13 == 4u || sqrt3m13 == 9u));
}