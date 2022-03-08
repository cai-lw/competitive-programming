#include "catch2/catch.hpp"
#include "cplib/port/bit.hpp"
#include "cplib/conv/multivar.hpp"
#include "utils.hpp"
using namespace std;
using namespace cplib;
using mint = MMInt<998244353>;

TEST_CASE("Small multivariate convolution", "[multivar]") {
    vector<int> a{1, 2, 3, 4, 5, 6}, b{7, 8, 9, 10, 11, 12};
    vector<size_t> shape{3, 2};
    vector<mint> am = from_int_vec<mint>(a), bm = from_int_vec<mint>(b);
    am = multiply_multivar_fps(am, bm, shape);
    a = to_int_vec(am);
    REQUIRE(a == vector<int>{7, 22, 30, 80, 73, 182});
}

TEST_CASE("Large multivariate convolution", "[multivar]") {
    int DIM = 10;
    vector<int> a(1 << DIM, 1), b(1 << DIM, 1);
    vector<size_t> shape(DIM, 2);
    vector<mint> am = from_int_vec<mint>(a), bm = from_int_vec<mint>(b);
    am = multiply_multivar_fps(am, bm, shape);
    a = to_int_vec(am);
    for (size_t i = 0; i < (1 << DIM); i++) {
        REQUIRE(a[i] == 1 << port::popcount(i));
    }
}