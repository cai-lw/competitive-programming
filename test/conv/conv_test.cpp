#include "catch2/catch.hpp"
#include "cplib/conv/conv.hpp"
using namespace std;
using namespace cplib;
using mint = MMInt<998244353>;

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

TEST_CASE("Small convolution", "conv") {
    using mint = MMInt<998244353>;
    vector<int> a{1, 2, 3, 4}, b{5, 6, 7, 8, 9};
    vector<mint> am = int2mint(a), bm = int2mint(b);
    convolution_inplace2(am, bm);
    a = mint2int(am);
    REQUIRE(a == vector<int>{5, 16, 34, 60, 70, 70, 59, 36});
}

TEST_CASE("Large convolution", "conv") {
    using mint = MMInt<998244353>;
    vector<int> a(123, 1), b(456, 1);
    vector<mint> am = int2mint(a), bm = int2mint(b);
    convolution_inplace2(am, bm);
    a = mint2int(am);
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