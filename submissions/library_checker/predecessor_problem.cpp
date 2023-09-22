#define PROBLEM "https://judge.yosupo.jp/problem/predecessor_problem"

#include <bits/stdc++.h>

#include "cplib/order/bit_trie.hpp"
using namespace std;
using namespace cplib;

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  int n, q;
  cin >> n >> q;
  BitTrie<uint32_t, 24> trie;
  string s;
  s.reserve(n);
  cin >> s;
  for (uint32_t i = 0; i < n; i++) {
    if (s[i] == '1') {
      trie.insert(i);
    }
  }
  while (q--) {
    int t;
    uint32_t k;
    cin >> t >> k;
    if (t == 0) {
      trie.insert(k);
    } else if (t == 1) {
      trie.erase(k);
    } else if (t == 2) {
      cout << trie.find(k) << '\n';
    } else {
      std::optional<uint32_t> ans = t == 3 ? trie.next(k) : trie.prev(k);
      cout << (ans ? int(*ans) : -1) << '\n';
    }
  }
}