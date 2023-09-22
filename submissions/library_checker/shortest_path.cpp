#define PROBLEM "https://judge.yosupo.jp/problem/shortest_path"

#include <bits/stdc++.h>

#include "cplib/order/dary_heap.hpp"
using namespace std;
using namespace cplib;

int main() {
  ios_base::sync_with_stdio(false);
  cin.tie(nullptr);
  int n, m, s, t;
  cin >> n >> m >> s >> t;
  vector<vector<pair<int, int>>> adj(n);
  for (int i = 0; i < m; i++) {
    int u, v, w;
    cin >> u >> v >> w;
    adj[u].emplace_back(v, w);
  }
  vector<long long> dist(n, 1e18);
  vector<int> prev(n, -1);
  dist[s] = 0;
  DaryHeap<pair<long long, int>> pq;
  pq.push({0, s});
  while (!pq.empty()) {
    auto [d, u] = pq.top();
    pq.pop();
    if (d > dist[u]) {
      continue;
    }
    if (u == t) {
      break;
    }
    for (auto [v, w] : adj[u]) {
      if (dist[v] > dist[u] + w) {
        dist[v] = dist[u] + w;
        prev[v] = u;
        pq.push({dist[v], v});
      }
    }
  }
  if (prev[t] == -1) {
    cout << "-1\n";
    return 0;
  }
  vector<int> path{t};
  while (path.back() != s) {
    path.push_back(prev[path.back()]);
  }
  reverse(path.begin(), path.end());
  cout << dist[t] << ' ' << path.size() - 1 << '\n';
  for (int i = 1; i < path.size(); i++) {
    cout << path[i - 1] << ' ' << path[i] << '\n';
  }
}