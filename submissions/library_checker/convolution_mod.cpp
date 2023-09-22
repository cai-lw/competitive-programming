#define PROBLEM "https://judge.yosupo.jp/problem/convolution_mod"

#include <bits/stdc++.h>

#include "cplib/conv/conv.hpp"
#include "cplib/num/mmint.hpp"
using namespace std;
using namespace cplib;
using mint = MMInt<998244353>;

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
  convolve_inplace2(a, b);
  for (size_t i = 0; i < n + m - 1; i++) {
    cout << a[i].val() << (i == n + m - 2 ? '\n' : ' ');
  }
}