#include "cplib/num/prime.hpp"

#include "catch2/catch_test_macros.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Primality test", "[prime]") {
  CHECK(!is_prime(0u));
  CHECK(!is_prime(1u));
  CHECK(is_prime(2u));
  CHECK(is_prime(13u));
  CHECK(!is_prime(42u));
  CHECK(!is_prime(10001u));
  CHECK(is_prime(10007u));
  CHECK(is_prime(1000000007u));
  CHECK(is_prime(2147483647u));
  CHECK(!is_prime(4294967295u));
  CHECK(!is_prime(998244353ull * 1000000007ull));
  CHECK(is_prime((1ull << 61) - 1));
  CHECK(!is_prime(0xFFFFFFFFFFFFFFFFull));
}