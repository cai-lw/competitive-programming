#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "cplib/num/mmint.hpp"
#include "cplib/num/primitive_root.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Primitive root modulo small numbers", "[primitive_root]") {
    CHECK_FALSE(primitive_root(0u));
    CHECK_FALSE(primitive_root(1u));
    CHECK(primitive_root_prime(2u) == 1u);
    CHECK(primitive_root_prime(3u) == 2u);
    CHECK(primitive_root(4u) == 3u);
    auto g5 = primitive_root_prime(5u);
    CHECK((g5 == 2u || g5 == 3u));
    CHECK(primitive_root(6u) == 5u);
    auto g9 = primitive_root(9u);
    CHECK((g9 == 2u || g9 == 5u));
    CHECK_FALSE(primitive_root(15u));
    auto g18 = primitive_root(18u);
    CHECK((g18 == 5u || g18 == 11u));
}

TEST_CASE("Primitive root modulo large 32-bit numbers", "[primitive_root]") {
    struct TestParam {
        uint32_t n;
        uint32_t phi;
        std::vector<uint32_t> phi_factors;
    };
    auto param = GENERATE(
        TestParam{1000000007, 1000000006, {2, 500000003}},
        TestParam{2000000014, 1000000006, {2, 500000003}},
        TestParam{4294967291, 4294967290, {2, 5, 19, 22605091}},
        TestParam{4293001441 /* =65521^2 */, 4292935920, {2, 3, 5, 7, 13, 65521}},
        TestParam{4259406061 /* =1621^3 */, 4256778420, {2, 3, 5, 1621}},
        TestParam{4283401138 /* =2*1289^3 */, 2140039048, {2, 7, 23, 1289}},
        TestParam{3939040643 /* =83^5 */, 3891582322, {2, 41, 83}},
        TestParam{3486784401 /* =3^20 */, 2324522934, {2, 3}}
    );
    using mint = DynamicMMInt32;
    auto result = primitive_root(param.n);
    REQUIRE(result);
    if (param.n % 2 == 0) {
        return;  // TODO: remove this after we have modint that supports even modulus
    }
    auto _guard = mint::set_mod_guard(param.n);
    mint g(*result);
    CHECK(pow(g, param.phi) == mint(1));
    for (uint32_t f : param.phi_factors) {
        REQUIRE(param.phi % f == 0);
        CHECK(pow(g, param.phi / f) != mint(1));
    }
}

TEST_CASE("No primitive root", "[primitive_root]") {
    CHECK_FALSE(primitive_root(3u * 1000000007u));
    CHECK_FALSE(primitive_root(4u * 1000000007u));
    CHECK_FALSE(primitive_root(65519u * 65521u));
}