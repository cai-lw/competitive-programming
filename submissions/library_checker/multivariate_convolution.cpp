#define PROBLEM "https://judge.yosupo.jp/problem/multivariate_convolution"

#include <bits/stdc++.h>

#include "cplib/conv/multivar.hpp"
#include "cplib/num/mmint.hpp"
using namespace std;
using namespace cplib;
using mint = MMInt<998244353>;

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  int k;
  cin >> k;
  vector<size_t> shape(k);
  size_t n = 1;
  for (auto& d : shape) {
    cin >> d;
    n *= d;
  }
  vector<mint> a, b;
  a.reserve(n);
  for (size_t i = 0; i < n; i++) {
    unsigned int x;
    cin >> x;
    a.emplace_back(x);
  }
  b.reserve(n);
  for (size_t i = 0; i < n; i++) {
    unsigned int x;
    cin >> x;
    b.emplace_back(x);
  }
  reverse(shape.begin(), shape.end());
  a = multivariate_convolution(a, b, shape);
  for (int i = 0; i < a.size(); i++) {
    cout << a[i].val() << (i == a.size() - 1 ? '\n' : ' ');
  }
}