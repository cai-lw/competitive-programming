#include "catch2/catch.hpp"
#include "cplib/num/prime.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Primality test", "[prime]") {
    REQUIRE(!is_prime(0u));
    REQUIRE(!is_prime(1u));
    REQUIRE(is_prime(2u));
    REQUIRE(is_prime(13u));
    REQUIRE(!is_prime(42u));
    REQUIRE(!is_prime(10001u));
    REQUIRE(is_prime(10007u));
    REQUIRE(is_prime(1000000007u));
    REQUIRE(is_prime(2147483647u));
    REQUIRE(!is_prime(4294967295u));
    REQUIRE(!is_prime(998244353ull * 1000000007ull));
    REQUIRE(is_prime((1ull << 61) - 1));
    REQUIRE(!is_prime(0xFFFFFFFFFFFFFFFFull));
}