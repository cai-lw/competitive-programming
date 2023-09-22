#include "cplib/num/factor.hpp"

#include <cstdint>

#include "catch2/catch_test_macros.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Integer factorization", "[factor]") {
  CHECK(factorize(0u).empty());
  CHECK(factorize(1u).empty());
  CHECK(factorize(2u) == vector<unsigned int>{2u});
  CHECK(factorize(13u) == vector<unsigned int>{13u});
  CHECK(factorize(42u) == vector<unsigned int>{2u, 3u, 7u});
  CHECK(factorize(64u) == vector<unsigned int>(6, 2u));
  CHECK(factorize(729u) == vector<unsigned int>(6, 3u));
  CHECK(factorize(2022u) == vector<unsigned int>{2u, 3u, 337u});
  CHECK(factorize(10001u) == vector<unsigned int>{73u, 137u});
  CHECK(factorize(10007u) == vector<unsigned int>{10007u});
  CHECK(factorize(1000000007u) == vector<unsigned int>{1000000007u});
  CHECK(factorize(4294967295u) == vector<unsigned int>{3u, 5u, 17u, 257u, 65537u});
  CHECK(factorize((1ull << 61) - 1) == vector<unsigned long long>{(1ull << 61) - 1});
  CHECK(factorize(1ull << 61) == vector<unsigned long long>(61, 2ull));
  CHECK(factorize(10000000000000000001ull) == vector<unsigned long long>{11ull, 909090909090909091ull});
}