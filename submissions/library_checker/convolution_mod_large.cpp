#define PROBLEM "https://judge.yosupo.jp/problem/convolution_mod_large"

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
  const size_t MAX_LEN = 1 << 22;
  size_t padded_out_size = 1 << port::bit_width(min(n, MAX_LEN) + min(m, MAX_LEN) - 2);
  vector<vector<mint>> a((n - 1) / MAX_LEN + 1, vector<mint>(padded_out_size)),
      b((m - 1) / MAX_LEN + 1, vector<mint>(padded_out_size));
  for (size_t i = 0; i < n; i++) {
    unsigned int x;
    cin >> x;
    a[i / MAX_LEN][i % MAX_LEN] = mint(x);
  }
  for (size_t i = 0; i < m; i++) {
    unsigned int x;
    cin >> x;
    b[i / MAX_LEN][i % MAX_LEN] = mint(x);
  }
  for (auto &v : a) {
    fft_inplace(v);
  }
  for (auto &v : b) {
    fft_inplace(v);
  }
  int la = a.size(), lb = b.size();
  a.resize(la + lb - 1, vector<mint>(padded_out_size));
  for (int i = la + lb - 1; i >= 0; i--) {
    if (i < la) {
      for (size_t k = 0; k < padded_out_size; k++) {
        a[i][k] *= b[0][k];
      }
    }
    for (int j = max(1, i + 1 - la); j < lb && j <= i; j++) {
      for (size_t k = 0; k < padded_out_size; k++) {
        a[i][k] += a[i - j][k] * b[j][k];
      }
    }
  }
  for (auto &v : a) {
    ifft_inplace(v);
  }
  for (size_t i = 0; i < n + m - 1; i++) {
    mint x(0);
    if (i / MAX_LEN < a.size()) {
      x += a[i / MAX_LEN][i % MAX_LEN];
    }
    if (i / MAX_LEN > 0) {
      x += a[i / MAX_LEN - 1][i % MAX_LEN + MAX_LEN];
    }
    cout << x.val() << (i == n + m - 2 ? '\n' : ' ');
  }
}