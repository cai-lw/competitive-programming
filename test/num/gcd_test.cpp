#include <tuple>
#include "catch2/catch.hpp"
#include "cplib/num/gcd.hpp"
using namespace cplib;

TEST_CASE("GCD", "[gcd]") {
    REQUIRE(gcd(0u, 0u) == 0u);
    REQUIRE(gcd(0u, 42u) == 42u);
    REQUIRE(gcd(69u, 0u) == 69u);
    REQUIRE(gcd(42u, 69u) == 3u);
    REQUIRE(gcd(42u, 24u) == 6u);
    REQUIRE(gcd(42u, 14u) == 14u);
    REQUIRE(gcd(998244353u, 1000000007u) == 1u);
    REQUIRE(gcd(2147483647u, 4294967295u) == 1u);
    REQUIRE(gcd(1073741824u, 2147483648u) == 1073741824u);
    REQUIRE(gcd(4294967295u, 4294967295u) == 4294967295u);
}

TEST_CASE("Extended GCD", "[gcd]") {
    REQUIRE(bezout(0u, 0u) == std::make_tuple(0, 0, 0u));
    REQUIRE(bezout(0u, 42u) == std::make_tuple(0, 1, 42u));
    REQUIRE(bezout(69u, 0u) == std::make_tuple(1, 0, 69u));
    REQUIRE(bezout(42u, 69u) == std::make_tuple(5, -3, 3u));
    REQUIRE(bezout(42u, 24u) == std::make_tuple(-1, 2, 6u));
    REQUIRE(bezout(42u, 14u) == std::make_tuple(0, 1, 14u));
    REQUIRE(bezout(123456789u, 987654321u) == std::make_tuple(-8, 1, 9u));
    REQUIRE(bezout(4294967295u, 2u) == std::make_tuple(1, -2147483647, 1u));
    REQUIRE(bezout(4294967295u, 2147483647u) == std::make_tuple(1, -2, 1u));
    REQUIRE(bezout(4294967295u, 4294967293u) == std::make_tuple(-2147483646, 2147483647, 1u));
    REQUIRE(bezout(4294967295u, 4294967295u) == std::make_tuple(0, 1, 4294967295u));
}

TEST_CASE("Modular inverse", "[gcd]") {
    REQUIRE(mod_inverse(0u, 1u) == 0u);
    REQUIRE(mod_inverse(42u, 1u) == 0u);
    REQUIRE(mod_inverse(69u, 2u) == 1u);
    REQUIRE(mod_inverse(2u, 5u) == 3u);
    REQUIRE(mod_inverse(1u, 6u) == 1u);
    REQUIRE(mod_inverse(5u, 6u) == 5u);
    REQUIRE(mod_inverse(2u, 998244353u) == 499122177u);
    REQUIRE(mod_inverse(3u, 998244353u) == 332748118u);
    REQUIRE(mod_inverse(4u, 998244353u) == 748683265u);
    REQUIRE(mod_inverse(6u, 998244353u) == 166374059u);
    REQUIRE(mod_inverse(65536u, 2147483647u) == 32768u);
    REQUIRE(mod_inverse(65536u, 4294967295u) == 65536u);
    REQUIRE(mod_inverse(4294967294u, 4294967295u) == 4294967294u);
}