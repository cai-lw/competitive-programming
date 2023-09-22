#include <tuple>
#include "catch2/catch_test_macros.hpp"
#include "cplib/num/gcd.hpp"
using namespace cplib;

TEST_CASE("GCD", "[gcd]") {
    CHECK(gcd(0u, 0u) == 0u);
    CHECK(gcd(0u, 42u) == 42u);
    CHECK(gcd(69u, 0u) == 69u);
    CHECK(gcd(42u, 69u) == 3u);
    CHECK(gcd(42u, 24u) == 6u);
    CHECK(gcd(42u, 14u) == 14u);
    CHECK(gcd(998244353u, 1000000007u) == 1u);
    CHECK(gcd(2147483647u, 4294967295u) == 1u);
    CHECK(gcd(1073741824u, 2147483648u) == 1073741824u);
    CHECK(gcd(4294967295u, 4294967295u) == 4294967295u);
}

TEST_CASE("Extended GCD", "[gcd]") {
    CHECK(bezout(0u, 0u) == std::make_tuple(0, 0, 0u));
    CHECK(bezout(0u, 42u) == std::make_tuple(0, 1, 42u));
    CHECK(bezout(69u, 0u) == std::make_tuple(1, 0, 69u));
    CHECK(bezout(42u, 69u) == std::make_tuple(5, -3, 3u));
    CHECK(bezout(42u, 24u) == std::make_tuple(-1, 2, 6u));
    CHECK(bezout(42u, 14u) == std::make_tuple(0, 1, 14u));
    CHECK(bezout(123456789u, 987654321u) == std::make_tuple(-8, 1, 9u));
    CHECK(bezout(4294967295u, 2u) == std::make_tuple(1, -2147483647, 1u));
    CHECK(bezout(4294967295u, 2147483647u) == std::make_tuple(1, -2, 1u));
    CHECK(bezout(4294967295u, 4294967293u) == std::make_tuple(-2147483646, 2147483647, 1u));
    CHECK(bezout(4294967295u, 4294967295u) == std::make_tuple(0, 1, 4294967295u));
}

TEST_CASE("Modular inverse", "[gcd]") {
    CHECK(mod_inverse(0u, 1u) == 0u);
    CHECK(mod_inverse(42u, 1u) == 0u);
    CHECK(mod_inverse(69u, 2u) == 1u);
    CHECK(mod_inverse(2u, 5u) == 3u);
    CHECK(mod_inverse(1u, 6u) == 1u);
    CHECK(mod_inverse(5u, 6u) == 5u);
    CHECK(mod_inverse(2u, 998244353u) == 499122177u);
    CHECK(mod_inverse(3u, 998244353u) == 332748118u);
    CHECK(mod_inverse(4u, 998244353u) == 748683265u);
    CHECK(mod_inverse(6u, 998244353u) == 166374059u);
    CHECK(mod_inverse(65536u, 2147483647u) == 32768u);
    CHECK(mod_inverse(65536u, 4294967295u) == 65536u);
    CHECK(mod_inverse(4294967294u, 4294967295u) == 4294967294u);
}