#include "catch2/catch.hpp"
#include "cplib/num/mmint.hpp"
#include "cplib/conv/conv.hpp"
#include "utils.hpp"
using namespace std;
using namespace cplib;
using mint = MMInt<998244353>;

TEST_CASE("Small convolution", "[conv]") {
    vector<int> a{1, 2, 3, 4}, b{5, 6, 7, 8, 9};
    vector<mint> am = from_int_vec<mint>(a), bm = from_int_vec<mint>(b);
    convolve_inplace2(am, bm);
    a = to_int_vec(am);
    REQUIRE(a == vector<int>{5, 16, 34, 60, 70, 70, 59, 36});
}

TEST_CASE("Large convolution", "[conv]") {
    vector<int> a(123, 1), b(456, 1);
    vector<mint> am = from_int_vec<mint>(a), bm = from_int_vec<mint>(b);
    convolve_inplace2(am, bm);
    a = to_int_vec(am);
    for (int i = 0; i < 123; i++) {
        REQUIRE(a[i] == i + 1);
    }
    for (int i = 123; i < 456; i++) {
        REQUIRE(a[i] == 123);
    }
    for (int i = 456; i < 578; i++) {
        REQUIRE(a[i] == 578 - i);
    }
}