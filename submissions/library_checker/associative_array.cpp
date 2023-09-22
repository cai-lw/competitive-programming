#define PROBLEM "https://judge.yosupo.jp/problem/associative_array"

#include <bits/stdc++.h>

#include "cplib/hash/table.hpp"
using namespace std;
using namespace cplib;

struct KeyValue {
  uint64_t key;
  uint64_t value;
  bool operator==(const KeyValue &rhs) const { return key == rhs.key; }
};

template <>
struct cplib::WyHash<KeyValue> {
  WyHash<uint64_t> hash;
  size_t operator()(const KeyValue &x) const { return hash(x.key); }
};

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  int q;
  cin >> q;
  HashTable<KeyValue> table;
  table.reserve(q);
  while (q--) {
    uint16_t t;
    uint64_t k, v;
    cin >> t >> k;
    if (t == 0) {
      cin >> v;
      table.insert<true>(KeyValue{k, v});
    } else {
      auto cell = table.find_cell(KeyValue{k, 0});
      if (cell->occupied()) {
        cout << cell->value().value << '\n';
      } else {
        cout << "0\n";
      }
    }
  }
}