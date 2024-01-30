#define PROBLEM "https://judge.yosupo.jp/problem/static_range_inversions_query"

#include <bits/stdc++.h>

#include <atcoder/fenwicktree>

#include "cplib/range/batch_range_queries.hpp"
using namespace std;
using namespace cplib;

struct RangeInversionQuery {
  atcoder::fenwick_tree<unsigned int> ft;
  unsigned int size;
  unsigned long long inv;

  RangeInversionQuery(int n) : ft(n), size(n), inv(0) {}

  void push_front(int x) {
    inv += ft.sum(0, x);
    ft.add(x, 1);
  }

  void push_back(int x) {
    inv += ft.sum(x + 1, size);
    ft.add(x, 1);
  }

  void pop_front(int x) {
    inv -= ft.sum(0, x);
    ft.add(x, -1);
  }

  void pop_back(int x) {
    inv -= ft.sum(x + 1, size);
    ft.add(x, -1);
  }

  unsigned long long get() { return inv; }
};

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  int n, q;
  cin >> n >> q;
  vector<int> arr(n);
  for (auto& x : arr) {
    cin >> x;
  }
  vector<int> arr_unique = arr;
  sort(arr_unique.begin(), arr_unique.end());
  arr_unique.erase(unique(arr_unique.begin(), arr_unique.end()), arr_unique.end());
  for (auto& x : arr) {
    x = lower_bound(arr_unique.begin(), arr_unique.end(), x) - arr_unique.begin();
  }
  using Iter = vector<int>::iterator;
  vector<pair<Iter, Iter>> qs;
  qs.reserve(q);
  for (int i = 0; i < q; i++) {
    int l, r;
    cin >> l >> r;
    qs.emplace_back(arr.begin() + l, arr.begin() + r);
  }
  RangeInversionQuery range(arr_unique.size());
  auto ans = batch_range_queries(range, qs);
  for (auto x : ans) {
    cout << x << '\n';
  }
}