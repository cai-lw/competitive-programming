#define PROBLEM "https://judge.yosupo.jp/problem/discrete_logarithm_mod"

#include "cplib/num/discrete_log.hpp"

#include <bits/stdc++.h>
using namespace std;
using namespace cplib;

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  uint32_t t;
  cin >> t;
  while (t--) {
    uint32_t x, y, m;
    cin >> x >> y >> m;
    if (auto ret = discrete_log(x, y, m)) {
      cout << *ret << '\n';
    } else {
      cout << "-1\n";
    }
  }
}