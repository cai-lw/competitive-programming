#include <cstdint>
#include "catch2/catch.hpp"
#include "cplib/num/factor.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Integer factorization", "[factor]") {
    REQUIRE(factorize(0u).empty());
    REQUIRE(factorize(1u).empty());
    REQUIRE(factorize(2u) == vector<unsigned int>{2u});
    REQUIRE(factorize(13u) == vector<unsigned int>{13u});
    REQUIRE(factorize(42u) == vector<unsigned int>{2u, 3u, 7u});
    REQUIRE(factorize(64u) == vector<unsigned int>(6, 2u));
    REQUIRE(factorize(729u) == vector<unsigned int>(6, 3u));
    REQUIRE(factorize(2022u) == vector<unsigned int>{2u, 3u, 337u});
    REQUIRE(factorize(10001u) == vector<unsigned int>{73u, 137u});
    REQUIRE(factorize(10007u) == vector<unsigned int>{10007u});
    REQUIRE(factorize(1000000007u) == vector<unsigned int>{1000000007u});
    REQUIRE(factorize(4294967295u) == vector<unsigned int>{3u, 5u, 17u, 257u, 65537u});
    REQUIRE(factorize((1ull << 61) - 1) == vector<unsigned long long>{(1ull << 61) - 1});
    REQUIRE(factorize(1ull << 61) == vector<unsigned long long>(61, 2ull));
    REQUIRE(factorize(10000000000000000001ull) == vector<unsigned long long>{11ull, 909090909090909091ull});
}