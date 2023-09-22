#include <vector>
#include <random>
#include <algorithm>
#include "catch2/catch_test_macros.hpp"
#include "cplib/order/pairing_heap.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Pairing heap with small integers", "[pairing_heap]") {
    PairingHeap<int> heap;
    CHECK(heap.empty());
    auto it3 = heap.push(3);
    heap.push(1);
    heap.push(4);
    CHECK_FALSE(heap.empty());
    CHECK(heap.size() == 3);
    CHECK(heap.top() == 1);
    CHECK(*it3 == 3);
    heap.pop();
    CHECK(heap.size() == 2);
    CHECK(heap.top() == 3);
    CHECK(heap.begin() == it3);
    heap.push(1);
    auto it5 = heap.push(5);
    auto it9 = heap.push(9);
    CHECK(heap.size() == 5);
    CHECK(heap.top() == 1);
    heap.decrease_key(it9, 2);
    CHECK(*it9 == 2);
    CHECK(heap.top() == 1);
    heap.decrease_key(it5, 0);
    CHECK(heap.top() == 0);
    CHECK(heap.begin() == it5);
    heap.pop();
    CHECK(heap.top() == 1);
    heap.pop();
    CHECK(heap.top() == 2);
    CHECK(heap.begin() == it9);
    heap.decrease_key(it9, 1);
    CHECK(heap.top() == 1);
    heap.erase(it3);
    CHECK(heap.size() == 2);
    CHECK(heap.top() == 1);
    heap.pop();
    CHECK(heap.top() == 4);
    heap.pop();
    CHECK(heap.empty());
}

TEST_CASE("Pairing heap merging", "[pairing_heap]") {
    PairingHeap<int> heap1, heap2;
    heap1.push(1);
    heap2.push(2);
    heap1.push(3);
    heap2.push(4);
    CHECK(heap1.size() == 2);
    CHECK(heap1.top() == 1);
    CHECK(heap2.size() == 2);
    CHECK(heap2.top() == 2);
    heap1.merge(move(heap2));
    CHECK(heap1.top() == 1);
    CHECK(heap1.size() == 4);
    CHECK(heap2.empty());
    heap1.pop();
    CHECK(heap1.top() == 2);
    heap1.pop();
    CHECK(heap1.top() == 3);
    heap1.pop();
    CHECK(heap1.top() == 4);
    heap1.pop();
    CHECK(heap1.empty());
}