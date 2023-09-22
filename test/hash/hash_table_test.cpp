#include <string>

#include "catch2/catch_test_macros.hpp"
#include "cplib/hash/table.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Hash table with integers", "[hash_table]") {
  cplib::HashTable<int> table;
  REQUIRE(table.insert(123));
  REQUIRE(table.insert(456));
  REQUIRE(table.insert(789));
  CHECK(table.size() == 3);
  CHECK(table.contains(123));
  CHECK(table.contains(456));
  CHECK(table.contains(789));
  CHECK_FALSE(table.contains(321));
  REQUIRE_FALSE(table.insert(789));
  REQUIRE_FALSE(table.erase(654));
  REQUIRE(table.erase(456));
  CHECK(table.size() == 2);
  CHECK_FALSE(table.contains(456));
}

TEST_CASE("Hash table with strings", "[hash_table]") {
  cplib::HashTable<string> table;
  REQUIRE(table.insert("foo"));
  REQUIRE(table.insert("bar"));
  REQUIRE(table.insert("baz"));
  CHECK(table.size() == 3);
  CHECK(table.contains("foo"));
  CHECK(table.contains("bar"));
  CHECK(table.contains("baz"));
  CHECK_FALSE(table.contains("quux"));
  REQUIRE_FALSE(table.insert("bar"));
  REQUIRE_FALSE(table.erase("boo"));
  REQUIRE(table.erase("foo"));
  CHECK(table.size() == 2);
  CHECK_FALSE(table.contains("foo"));
}