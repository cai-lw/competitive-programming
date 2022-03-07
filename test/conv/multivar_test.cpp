#include "catch2/catch.hpp"
#include "cplib/port/bit.hpp"
#include "cplib/conv/multivar.hpp"
using namespace std;
using namespace cplib;
using mint = MMInt<998244353>;

namespace {

vector<mint> int2mint(const vector<int> &a) {
    vector<mint> res;
    for (int x : a) {
        res.push_back(mint(x));
    }
    return res;
}

vector<int> mint2int(const vector<mint> &a) {
    vector<int> res;
    for (const mint &x : a) {
        res.push_back(x.val());
    }
    return res;
}

}  // namespace

TEST_CASE("Small multivariate convolution", "[multivar]") {
    using mint = MMInt<998244353>;
    vector<int> a{1, 2, 3, 4, 5, 6}, b{7, 8, 9, 10, 11, 12};
    vector<size_t> shape{3, 2};
    vector<mint> am = int2mint(a), bm = int2mint(b);
    am = multiply_multivar_fps(am, bm, shape);
    a = mint2int(am);
    REQUIRE(a == vector<int>{7, 22, 30, 80, 73, 182});
}

TEST_CASE("Large multivariate convolution", "[multivar]") {
    using mint = MMInt<998244353>;
    int DIM = 10;
    vector<int> a(1 << DIM, 1), b(1 << DIM, 1);
    vector<size_t> shape(DIM, 2);
    vector<mint> am = int2mint(a), bm = int2mint(b);
    am = multiply_multivar_fps(am, bm, shape);
    a = mint2int(am);
    for (size_t i = 0; i < (1 << DIM); i++) {
        REQUIRE(a[i] == 1 << port::popcount(i));
    }
}