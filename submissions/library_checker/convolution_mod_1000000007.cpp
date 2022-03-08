#define PROBLEM "https://judge.yosupo.jp/problem/convolution_mod_1000000007"

#include <bits/stdc++.h>
#include "cplib/num/mmint.hpp"
#include "cplib/conv/anymod.hpp"
using namespace std;
using namespace cplib;
using mint = MMInt<1000000007>;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    size_t n, m;
    cin >> n >> m;
    vector<mint> a, b;
    a.reserve(n);
    for (size_t i = 0; i < n; i++) {
        unsigned int x;
        cin >> x;
        a.emplace_back(x);
    }
    b.reserve(m);
    for (size_t i = 0; i < m; i++) {
        unsigned int x;
        cin >> x;
        b.emplace_back(x);
    }
    a = convolve_any_modint(a, b);
    for (size_t i = 0; i < n + m - 1; i++) {
        cout << a[i].val() << (i == n + m - 2 ? '\n' : ' ');
    }
}