#include "cplib/range/wavelet_array.hpp"

#include <vector>

#include "catch2/catch_test_macros.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Small wavelet array", "[wavelet_array]") {
  vector<uint8_t> data{3, 1, 4, 1, 5, 9, 2, 6, 5};
  WaveletArray<uint8_t, 4> wa(std::move(data));
  CHECK(wa.size() == 9);
  CHECK(wa.get(3) == 1);
  CHECK(wa.get(7) == 6);
  CHECK(wa.range_nth(0, 3, 0) == 1);
  CHECK(wa.range_nth(0, 3, 1) == 3);
  CHECK(wa.range_nth(0, 3, 2) == 4);
  CHECK(wa.range_nth(4, 9, 1) == 5);
  CHECK(wa.range_nth(4, 9, 2) == 5);
  CHECK(wa.range_nth(4, 9, 4) == 9);
  CHECK(wa.range_nth(7, 8, 0) == 6);
  CHECK(wa.range_nth(0, 9, 2) == 2);
  CHECK(wa.range_count(0, 3, 1) == 1);
  CHECK(wa.range_count(3, 7, 1) == 1);
  CHECK(wa.range_count(4, 9, 1) == 0);
  CHECK(wa.range_count(4, 9, 5) == 2);
  CHECK(wa.range_count(5, 6, 9) == 1);
  CHECK(wa.range_count(7, 8, 9) == 0);
  CHECK(wa.range_count(7, 8, 6) == 1);
  CHECK(wa.range_count(0, 9, 1) == 2);
  CHECK(wa.range_count_between(0, 5, 3, 5) == 3);
  CHECK(wa.range_count_between(0, 5, 1, 2) == 2);
  CHECK(wa.range_count_between(0, 5, 2, 2) == 0);
  CHECK(wa.range_count_between(0, 5, 1, 9) == 5);
  CHECK(wa.range_count_between(0, 5, 6, 9) == 0);
  CHECK(wa.range_count_between(3, 6, 2, 4) == 0);
  CHECK(wa.range_count_between(3, 6, 4, 8) == 1);
  CHECK(wa.range_count_between(5, 9, 5, 6) == 2);
  CHECK(wa.range_count_between(6, 7, 2, 2) == 1);
  CHECK(wa.range_count_between(0, 9, 3, 5) == 4);
}