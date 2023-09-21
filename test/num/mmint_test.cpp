#include "catch2/catch.hpp"
#include "cplib/num/mmint.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Basic arithmetics with Montgomery modular integer", "[mmint]") {
    using mint = MMInt<998244353>;
    REQUIRE(mint::mod() == 998244353u);
    REQUIRE(mint().val() == 0u);
    REQUIRE(mint(42).val() == 42u);
    REQUIRE(mint(1000000007).val() == 1755654u);
    REQUIRE(mint(9982443530123456789ull).val() == 123456789u);
    REQUIRE(mint(-1).val() == 998244352u);
    REQUIRE(mint(-998244353).val() == 0u);
    REQUIRE((mint(3) + mint(2)).val() == 5u);
    REQUIRE((mint(987654321) + mint(12345678)).val() == 1755646u);
    REQUIRE((-mint(3)).val() == 998244350u);
    REQUIRE((mint(3) - mint(2)).val() == 1u);
    REQUIRE((mint(3) - mint(4)).val() == 998244352u);
    REQUIRE((mint(3) * mint(2)).val() == 6u);
    REQUIRE((mint(10000) * mint(100000)).val() == 1755647u);
    REQUIRE((mint(3) * mint(-2)).val() == 998244347u);
    REQUIRE((mint(42) / mint(6)).val() == 7u);
    REQUIRE(mint(3).inv().val() == 332748118u);
    REQUIRE((mint(3) / mint(6)).val() == 499122177u);
    REQUIRE((mint(3) / mint(-6)).val() == 499122176u);
    REQUIRE(mint(42) == mint(42));
    REQUIRE(mint(-1) == mint(998244352));
    REQUIRE(mint(42) != mint(69));
    mint test(42);
    REQUIRE((++test).val() == 43u);
    REQUIRE((test++).val() == 43u);
    REQUIRE(test.val() == 44u);
    REQUIRE((--test).val() == 43u);
    REQUIRE((test--).val() == 43u);
    REQUIRE(test.val() == 42u);
    REQUIRE((test += mint(3)).val() == 45u);
    REQUIRE((test /= mint(5)).val() == 9u);
    REQUIRE((test -= mint(2)).val() == 7u);
    REQUIRE((test *= mint(6)).val() == 42u);
}

TEMPLATE_TEST_CASE("Identities with Montgomery modular integer", "[mmint]", MMInt<998244353>, MMInt<4294967291>) {
    using mint = TestType;
    // Factorial
    unsigned long long fac = 1;
    mint mfac(1);
    for (int i = 1; i <= 20; i++) {
        fac *= i;
        mfac *= mint(i);
        REQUIRE(mfac.val() == fac % mint::mod());
    }
    for (int i = 1; i <= 20; i++) {
        fac /= i;
        mfac /= mint(i);
        REQUIRE(mfac.val() == fac % mint::mod());
    }
    // Sum of squares
    unsigned long long sum = 0;
    mint msum(0);
    for (int i = 1; i <= 2000; i++) {
        sum += i * i;
        msum += mint(i) * mint(i);
        REQUIRE(msum.val() == sum % mint::mod());
        mint msum_direct = mint(i) * mint(i + 1) * mint(2 * i + 1) / mint(6);
        REQUIRE(msum == msum_direct);
        REQUIRE(msum_direct.val() == sum % mint::mod());
    }
}

TEST_CASE("Dynamic Montgomery modular integer", "[mmint]") {
    using mint = DynamicMMInt30;
    // Start as mod 11
    auto _guard = mint::set_mod_guard(11);
    REQUIRE(mint::mod() == 11u);
    REQUIRE((mint(6) + mint(7)).val() == 2u);
    REQUIRE((mint(8) - mint(9)).val() == 10u);
    // Temporarily set mod to 13 in this scope
    {
        auto _guard = mint::set_mod_guard(13);
        REQUIRE(mint::mod() == 13u);
        REQUIRE((mint(6) + mint(7)).val() == 0u);
        REQUIRE((mint(8) - mint(9)).val() == 12u);
        REQUIRE((mint(4) * mint(5)).val() == 7u);
        REQUIRE((mint(2) / mint(3)).val() == 5u);
    }
    // Out of scope and back to mod 11
    REQUIRE(mint::mod() == 11u);
    REQUIRE((mint(4) * mint(5)).val() == 9u);
    REQUIRE((mint(2) / mint(3)).val() == 8u);
}