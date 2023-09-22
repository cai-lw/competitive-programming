#define PROBLEM "https://judge.yosupo.jp/problem/range_kth_smallest"

#include <bits/stdc++.h>

#include "cplib/range/wavelet_array.hpp"
using namespace std;
using namespace cplib;

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  int n, q;
  cin >> n >> q;
  vector<uint32_t> a(n);
  for (auto& x : a) {
    cin >> x;
  }
  WaveletArray<uint32_t, 30> wa(move(a));
  while (q--) {
    uint32_t l, r, k;
    cin >> l >> r >> k;
    cout << wa.range_nth(l, r, k) << '\n';
  }
}