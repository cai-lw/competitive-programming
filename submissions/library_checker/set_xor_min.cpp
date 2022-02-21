#define PROBLEM "https://judge.yosupo.jp/problem/set_xor_min"

#include <bits/stdc++.h>
#include "cplib/order/bit_trie.hpp"
using namespace std;
using namespace cplib;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int q;
    cin >> q;
    BitTrie<uint32_t, 30> trie;
    while (q--) {
        int t;
        uint32_t x;
        cin >> t >> x;
        if (t == 0) {
            trie.insert(x);
        } else if (t == 1) {
            trie.erase(x);
        } else {
            cout << trie.xor_min(x) << '\n';
        }
    }
}