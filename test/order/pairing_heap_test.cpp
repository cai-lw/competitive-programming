#include <vector>
#include <random>
#include <algorithm>
#include "catch2/catch.hpp"
#include "cplib/order/pairing_heap.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Pairing heap with small integers", "[pairing_heap]") {
    PairingHeap<int> heap;
    REQUIRE(heap.empty());
    auto it3 = heap.push(3);
    heap.push(1);
    heap.push(4);
    REQUIRE(!heap.empty());
    REQUIRE(heap.size() == 3);
    REQUIRE(heap.top() == 1);
    REQUIRE(*it3 == 3);
    heap.pop();
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.top() == 3);
    REQUIRE(heap.begin() == it3);
    heap.push(1);
    auto it5 = heap.push(5);
    auto it9 = heap.push(9);
    REQUIRE(heap.size() == 5);
    REQUIRE(heap.top() == 1);
    heap.decrease_key(it9, 2);
    REQUIRE(*it9 == 2);
    REQUIRE(heap.top() == 1);
    heap.decrease_key(it5, 0);
    REQUIRE(heap.top() == 0);
    REQUIRE(heap.begin() == it5);
    heap.pop();
    REQUIRE(heap.top() == 1);
    heap.pop();
    REQUIRE(heap.top() == 2);
    REQUIRE(heap.begin() == it9);
    heap.decrease_key(it9, 1);
    REQUIRE(heap.top() == 1);
    heap.erase(it3);
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.top() == 1);
    heap.pop();
    REQUIRE(heap.top() == 4);
    heap.pop();
    REQUIRE(heap.empty());
}

TEST_CASE("Pairing heap merging", "[pairing_heap]") {
    PairingHeap<int> heap1, heap2;
    heap1.push(1);
    heap2.push(2);
    heap1.push(3);
    heap2.push(4);
    REQUIRE(heap1.size() == 2);
    REQUIRE(heap1.top() == 1);
    REQUIRE(heap2.size() == 2);
    REQUIRE(heap2.top() == 2);
    heap1.merge(move(heap2));
    REQUIRE(heap1.top() == 1);
    REQUIRE(heap1.size() == 4);
    REQUIRE(heap2.empty());
    heap1.pop();
    REQUIRE(heap1.top() == 2);
    heap1.pop();
    REQUIRE(heap1.top() == 3);
    heap1.pop();
    REQUIRE(heap1.top() == 4);
    heap1.pop();
    REQUIRE(heap1.empty());
}