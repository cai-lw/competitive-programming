#define PROBLEM "https://judge.yosupo.jp/problem/staticrmq"

#include <bits/stdc++.h>
#include "cplib/range/rmq.hpp"
using namespace std;
using namespace cplib;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    size_t n, q;
    cin >> n >> q;
    vector<uint32_t> a(n);
    for(auto &x : a) {
        cin >> x;
    }
    RangeMinQuery<uint32_t> rmq(move(a));
    while (q--) {
        size_t l, r;
        cin >> l >> r;
        cout << rmq.range_min(l, r) << '\n';
    }
}