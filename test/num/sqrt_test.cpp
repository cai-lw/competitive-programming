#include "cplib/num/sqrt.hpp"

#include "catch2/catch_test_macros.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Modular sqaure root", "[sqrt]") {
  CHECK(sqrt_mod_prime(0u, 2u) == 0u);
  CHECK(sqrt_mod_prime(1u, 2u) == 1u);
  CHECK_FALSE(sqrt_mod_prime(3u, 7u));
  auto sqrt2m7 = sqrt_mod_prime(2u, 7u);
  CHECK((sqrt2m7 == 3u || sqrt2m7 == 4u));
  auto sqrt11m7 = sqrt_mod_prime(11u, 7u);
  CHECK((sqrt11m7 == 2u || sqrt11m7 == 5u));
  CHECK(sqrt_mod_prime(0u, 998244353u) == 0u);
  auto sqrt2m998244353 = sqrt_mod_prime(2u, 998244353u);
  CHECK((sqrt2m998244353 == 116195171u || sqrt2m998244353 == 998244353u - 116195171u));

  using mint = MMInt<13>;
  CHECK(sqrt_mod_fp(mint(0)) == mint(0));
  CHECK_FALSE(sqrt_mod_fp(mint(2)));
  auto sqrt3m13 = sqrt_mod_fp(mint(3));
  CHECK((sqrt3m13 == mint(4) || sqrt3m13 == mint(9)));
}