#include "cplib/range/batch_range_queries.hpp"

#include <vector>

#include "catch2/catch_test_macros.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Static range sum query") {
  struct RangeSum {
    int sum = 0;
    void push_front(int x) { sum += x; }
    void pop_front(int x) { sum -= x; }
    void push_back(int x) { sum += x; }
    void pop_back(int x) { sum -= x; }
    int get() const { return sum; }
  };
  vector<int> data{3, 1, 4, 1, 5, 9, 2, 6, 5};
  using Iter = vector<int>::iterator;
  vector<pair<Iter, Iter>> queries{
      {data.begin(), data.begin() + 3},     {data.end() - 2, data.end()}, {data.begin() + 4, data.begin() + 5},
      {data.begin() + 5, data.begin() + 5}, {data.begin(), data.end()},
  };
  RangeSum range;
  auto ans = batch_range_queries(range, queries);
  CHECK(ans == vector<int>{8, 11, 5, 0, 36});
}