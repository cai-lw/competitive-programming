#include <vector>
#include "catch2/catch.hpp"
#include "cplib/port/bit.hpp"
#include "cplib/conv/anymod.hpp"
#include "utils.hpp"
using namespace std;
using namespace cplib;
using mint = MMInt<1000000007>;

TEST_CASE("Small anymod convolution", "[anymod]") {
    vector<int> a{1, 2, 3, 4}, b{5, 6, 7, 8, 9};
    vector<size_t> shape{3, 2};
    vector<mint> am = from_int_vec<mint>(a), bm = from_int_vec<mint>(b);
    am = convolve_any_modint(am, bm);
    a = to_int_vec(am);
    REQUIRE(a == vector<int>{5, 16, 34, 60, 70, 70, 59, 36});
}

TEST_CASE("Large anymod convolution", "[anymod]") {
    const int N = 100, X = 987654321, X2 = (unsigned long long)X * X % mint::mod();
    vector<int> a(N, X), b(N, X);
    vector<mint> am = from_int_vec<mint>(a), bm = from_int_vec<mint>(b);
    am = convolve_any_modint(am, bm);
    a = to_int_vec(am);
    for (int i = 0; i < N * 2 - 1; i++) {
        REQUIRE(a[i] == (unsigned long long)min(i + 1, N * 2 - 1 - i) * X2 % mint::mod());
    }
}