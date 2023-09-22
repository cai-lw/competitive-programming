#include <vector>
#include <random>
#include <algorithm>
#include "catch2/catch_test_macros.hpp"
#include "cplib/order/dary_heap.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Small heap with small integers", "[dary_heap]") {
    DaryHeap<int> heap;
    CHECK(heap.empty());
    heap.push(3);
    heap.push(1);
    heap.push(4);
    CHECK_FALSE(heap.empty());
    CHECK(heap.size() == 3);
    CHECK(heap.top() == 1);
    heap.pop();
    CHECK(heap.size() == 2);
    CHECK(heap.top() == 3);
    heap.push(1);
    heap.push(5);
    heap.push(9);
    CHECK(heap.size() == 5);
    CHECK(heap.top() == 1);
    heap.pop();
    CHECK(heap.top() == 3);
    heap.pop();
    CHECK(heap.top() == 4);
    heap.pop();
    CHECK(heap.top() == 5);
    heap.pop();
    CHECK(heap.top() == 9);
    heap.pop();
    CHECK(heap.empty());
}

TEST_CASE("Heap with many integers inserted in random order", "[dary_heap]") {
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
        CHECK(heap.top() == i);
        CHECK(heap.size() == N - i);
        heap.pop();
    }
    CHECK(heap.empty());
}