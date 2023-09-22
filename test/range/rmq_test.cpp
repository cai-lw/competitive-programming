#include <vector>
#include <utility>
#include "catch2/catch.hpp"
#include "cplib/range/rmq.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("RMQ with small integers", "[rmq]") {
    vector<int> data{3, 1, 4, 1, 5, 9, 2, 6, 5};
    RangeMinQuery<int> rmq(move(data));
    CHECK(rmq.size() == 9);
    CHECK(rmq.range_min(0, 3) == 1);
    CHECK(rmq.range_min(4, 9) == 2);
    CHECK(rmq.range_min(4, 6) == 5);
    CHECK(rmq.range_min(7, 8) == 6);
}

TEST_CASE("RMQ with strings", "[rmq]") {
    vector<string> data{"one", "two", "three", "four", "five", "six"};
    RangeMinQuery<string> rmq(move(data));
    CHECK(rmq.size() == 6);
    CHECK(rmq.range_min(0, 6) == "five");
    CHECK(rmq.range_min(0, 3) == "one");
    CHECK(rmq.range_min(1, 3) == "three");
    CHECK(rmq.range_min(1, 4) == "four");
}

struct TwoAdicNormCompare {
    bool operator()(unsigned int a, unsigned int b) {
        return port::countr_zero(a) > port::countr_zero(b);
    }
};

TEST_CASE("RMQ with custom comparison operator", "[rmq]") {
    vector<unsigned int> data{4, 5, 6, 7, 8, 9, 10};
    RangeMinQuery<unsigned int, TwoAdicNormCompare> rmq(move(data));
    CHECK(rmq.size() == 7);
    CHECK(rmq.range_min(0, 7) == 8);
    CHECK(rmq.range_min(0, 4) == 4);
    CHECK(rmq.range_min(1, 4) == 6);
    CHECK(rmq.range_min(5, 6) == 9);
}

TEST_CASE("RMQ with large bitonic array", "[rmq]") {
    // 0 1 2 ... 499 500 499 ... 2 1 0
    vector<int> data(1001);
    iota(data.rbegin(), data.rend(), 0);
    iota(data.begin(), data.begin() + 500, 0);
    RangeMinQuery<int> rmq(move(data));
    CHECK(rmq.size() == 1001);
    CHECK(rmq.range_min(1, 1000) == 1);
    CHECK(rmq.range_min(123, 456) == 123);
    CHECK(rmq.range_min(456, 789) == 212);
    CHECK(rmq.range_min(333, 666) == 333);
    CHECK(rmq.range_min(666, 999) == 2);
    CHECK(rmq.range_min(495, 505) == 495);
}