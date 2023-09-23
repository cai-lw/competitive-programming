#define PROBLEM "https://judge.yosupo.jp/problem/primality_test"

#include <bits/stdc++.h>

#include "cplib/num/prime.hpp"
using namespace std;
using namespace cplib;

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  int q;
  cin >> q;
  while (q--) {
    uint64_t n;
    cin >> n;
    cout << (is_prime(n) ? "Yes\n" : "No\n");
  }
}