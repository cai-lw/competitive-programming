#include "catch2/catch_template_test_macros.hpp"
#include "catch2/catch_test_macros.hpp"
#include "cplib/num/mmint.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Basic arithmetics with Montgomery modular integer", "[mmint]") {
    using mint = MMInt<998244353>;
    CHECK(mint::mod() == 998244353u);
    CHECK(mint().val() == 0u);
    CHECK(mint(42).val() == 42u);
    CHECK(mint(1000000007).val() == 1755654u);
    CHECK(mint(9982443530123456789ull).val() == 123456789u);
    CHECK(mint(-1).val() == 998244352u);
    CHECK(mint(-998244353).val() == 0u);
    CHECK((mint(3) + mint(2)).val() == 5u);
    CHECK((mint(987654321) + mint(12345678)).val() == 1755646u);
    CHECK((-mint(3)).val() == 998244350u);
    CHECK((mint(3) - mint(2)).val() == 1u);
    CHECK((mint(3) - mint(4)).val() == 998244352u);
    CHECK((mint(3) * mint(2)).val() == 6u);
    CHECK((mint(10000) * mint(100000)).val() == 1755647u);
    CHECK((mint(3) * mint(-2)).val() == 998244347u);
    CHECK((mint(42) / mint(6)).val() == 7u);
    CHECK(mint(3).inv().val() == 332748118u);
    CHECK((mint(3) / mint(6)).val() == 499122177u);
    CHECK((mint(3) / mint(-6)).val() == 499122176u);
    CHECK(mint(42) == mint(42));
    CHECK(mint(-1) == mint(998244352));
    CHECK(mint(42) != mint(69));
    mint test(42);
    CHECK((++test).val() == 43u);
    CHECK((test++).val() == 43u);
    CHECK(test.val() == 44u);
    CHECK((--test).val() == 43u);
    CHECK((test--).val() == 43u);
    CHECK(test.val() == 42u);
    CHECK((test += mint(3)).val() == 45u);
    CHECK((test /= mint(5)).val() == 9u);
    CHECK((test -= mint(2)).val() == 7u);
    CHECK((test *= mint(6)).val() == 42u);
}

TEMPLATE_TEST_CASE("Identities with Montgomery modular integer", "[mmint]", MMInt<998244353>, MMInt<4294967291>) {
    using mint = TestType;
    // Factorial
    unsigned long long fac = 1;
    mint mfac(1);
    for (int i = 1; i <= 20; i++) {
        fac *= i;
        mfac *= mint(i);
        CHECK(mfac.val() == fac % mint::mod());
    }
    for (int i = 1; i <= 20; i++) {
        fac /= i;
        mfac /= mint(i);
        CHECK(mfac.val() == fac % mint::mod());
    }
    // Sum of squares
    unsigned long long sum = 0;
    mint msum(0);
    for (int i = 1; i <= 2000; i++) {
        sum += i * i;
        msum += mint(i) * mint(i);
        CHECK(msum.val() == sum % mint::mod());
        mint msum_direct = mint(i) * mint(i + 1) * mint(2 * i + 1) / mint(6);
        CHECK(msum == msum_direct);
        CHECK(msum_direct.val() == sum % mint::mod());
    }
}

TEST_CASE("Dynamic Montgomery modular integer", "[mmint]") {
    using mint = DynamicMMInt30;
    // Start as mod 11
    auto _guard = mint::set_mod_guard(11);
    CHECK(mint::mod() == 11u);
    CHECK((mint(6) + mint(7)).val() == 2u);
    CHECK((mint(8) - mint(9)).val() == 10u);
    // Temporarily set mod to 13 in this scope
    {
        auto _guard = mint::set_mod_guard(13);
        CHECK(mint::mod() == 13u);
        CHECK((mint(6) + mint(7)).val() == 0u);
        CHECK((mint(8) - mint(9)).val() == 12u);
        CHECK((mint(4) * mint(5)).val() == 7u);
        CHECK((mint(2) / mint(3)).val() == 5u);
    }
    // Out of scope and back to mod 11
    CHECK(mint::mod() == 11u);
    CHECK((mint(4) * mint(5)).val() == 9u);
    CHECK((mint(2) / mint(3)).val() == 8u);
}