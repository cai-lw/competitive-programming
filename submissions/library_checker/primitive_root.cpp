#define PROBLEM "https://judge.yosupo.jp/problem/primitive_root"

#include <bits/stdc++.h>
#include "cplib/num/primitive_root.hpp"
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
        cout << primitive_root_prime(a) << '\n';
    }
}