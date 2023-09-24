#include "cplib/num/discrete_log.hpp"

#include "catch2/catch_test_macros.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Discrete log", "[discrete_log]") {
  CHECK(discrete_log(0u, 0u, 1u) == 0);
  CHECK(discrete_log(1u, 1u, 2u) == 0);
  CHECK(discrete_log(0u, 1u, 3u) == 0);
  CHECK(discrete_log(0u, 0u, 3u) == 1);
  CHECK_FALSE(discrete_log(1u, 0u, 3u));
  CHECK_FALSE(discrete_log(1u, 2u, 3u));
  CHECK(discrete_log(2u, 1u, 3u) == 0);
  CHECK(discrete_log(2u, 2u, 3u) == 1);
  CHECK(discrete_log(2u, 3u, 5u) == 3);
  CHECK_FALSE(discrete_log(1u, 3u, 5u));
  CHECK_FALSE(discrete_log(4u, 3u, 5u));
  CHECK(discrete_log(2u, 6u, 10u) == 4);
  CHECK(discrete_log(6u, 6u, 10u) == 1);
  CHECK(discrete_log(2u, 1u, 10u) == 0);
  CHECK_FALSE(discrete_log(2u, 5u, 10u));
  CHECK(discrete_log(3u, 7u, 10u) == 3);
  CHECK_FALSE(discrete_log(3u, 6u, 10u));
}