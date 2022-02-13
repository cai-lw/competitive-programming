#include <string>
#include "catch2/catch.hpp"
#include "cplib/hash/table.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Hash table with integers", "[hash_table]") {
    cplib::HashTable<int> table;
    REQUIRE(table.insert(123));
    REQUIRE(table.insert(456));
    REQUIRE(table.insert(789));
    REQUIRE(table.size() == 3);
    REQUIRE(table.contains(123));
    REQUIRE(table.contains(456));
    REQUIRE(table.contains(789));
    REQUIRE(!table.contains(321));
    REQUIRE(!table.insert(789));
    REQUIRE(!table.erase(654));
    REQUIRE(table.erase(456));
    REQUIRE(table.size() == 2);
    REQUIRE(!table.contains(456));
}

TEST_CASE("Hash table with strings", "[hash_table]") {
    cplib::HashTable<string> table;
    REQUIRE(table.insert("foo"));
    REQUIRE(table.insert("bar"));
    REQUIRE(table.insert("baz"));
    REQUIRE(table.size() == 3);
    REQUIRE(table.contains("foo"));
    REQUIRE(table.contains("bar"));
    REQUIRE(table.contains("baz"));
    REQUIRE(!table.contains("quux"));
    REQUIRE(!table.insert("bar"));
    REQUIRE(!table.erase("boo"));
    REQUIRE(table.erase("foo"));
    REQUIRE(table.size() == 2);
    REQUIRE(!table.contains("foo"));
}