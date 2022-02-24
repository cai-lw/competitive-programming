#include <vector>
#include <random>
#include <algorithm>
#include "catch2/catch.hpp"
#include "cplib/order/dary_heap.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Small heap with small integers", "[dary_heap]") {
    DaryHeap<int> heap;
    REQUIRE(heap.empty());
    heap.push(3);
    heap.push(1);
    heap.push(4);
    REQUIRE(!heap.empty());
    REQUIRE(heap.size() == 3);
    REQUIRE(heap.top() == 1);
    heap.pop();
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.top() == 3);
    heap.push(1);
    heap.push(5);
    heap.push(9);
    REQUIRE(heap.size() == 5);
    REQUIRE(heap.top() == 1);
    heap.pop();
    REQUIRE(heap.top() == 3);
    heap.pop();
    REQUIRE(heap.top() == 4);
    heap.pop();
    REQUIRE(heap.top() == 5);
    heap.pop();
    REQUIRE(heap.top() == 9);
    heap.pop();
    REQUIRE(heap.empty());
}

TEST_CASE("Heap with many integers inserted in random order", "[bit_trie]") {
    const int N = 1000;
    vector<int> a(N);
    iota(a.begin(), a.end(), 0);
    mt19937 rng(42);
    shuffle(a.begin(), a.end(), rng);
    DaryHeap<int> heap;
    for (int i : a) {
        heap.push(i);
    }
    for (int i = 0; i < N; i++) {
        REQUIRE(heap.top() == i);
        REQUIRE(heap.size() == N - i);
        heap.pop();
    }
    REQUIRE(heap.empty());
}