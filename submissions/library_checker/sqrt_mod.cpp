#define PROBLEM "https://judge.yosupo.jp/problem/sqrt_mod"

#include <bits/stdc++.h>
#include "cplib/num/mod_math.hpp"
using namespace std;
using namespace cplib;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    uint32_t t;
    cin >> t;
    while (t--) {
        uint32_t y, p;
        cin >> y >> p;
        std::optional<uint32_t> ans = sqrt_mod_prime(y, p);
        cout << (ans ? int32_t(*ans) : -1) << '\n';
    }
}