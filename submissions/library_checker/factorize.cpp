#define PROBLEM "https://judge.yosupo.jp/problem/factorize"

#include <bits/stdc++.h>
#include "cplib/num/factor.hpp"
using namespace std;
using namespace cplib;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int q;
    cin >> q;
    while (q--) {
        uint64_t a;
        cin >> a;
        auto ps = factorize(a);
        cout << ps.size();
        for (auto p : ps) {
            cout << ' ' << p;
        }
        cout << '\n';
    }
}