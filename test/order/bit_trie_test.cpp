#include <vector>
#include "catch2/catch.hpp"
#include "cplib/order/bit_trie.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Small bit trie", "[bit_trie]") {
    BitTrie<uint8_t, 4> trie;
    CHECK(trie.empty());
    REQUIRE(trie.insert(3));
    REQUIRE(trie.insert(1));
    REQUIRE(trie.insert(4));
    REQUIRE_FALSE(trie.insert(1));
    REQUIRE(trie.insert(5));
    REQUIRE(trie.insert(9));
    // {1, 3, 4, 5, 9}
    CHECK(!trie.empty());
    CHECK(trie.size() == 5);
    CHECK(trie.find(5));
    CHECK(trie.find(9));
    CHECK_FALSE(trie.find(2));
    CHECK_FALSE(trie.find(15));
    CHECK(trie.next(3) == 3);
    CHECK(trie.next(6) == 9);
    CHECK_FALSE(trie.next(10));
    CHECK_FALSE(trie.prev(0));
    CHECK(trie.prev(2) == 1);
    CHECK(trie.prev(4) == 4);
    CHECK(trie.xor_min(3) == 0);
    CHECK(trie.xor_min(7) == 2);
    CHECK(trie.xor_min(14) == 7);
    REQUIRE(trie.erase(4));
    REQUIRE(trie.erase(5));
    REQUIRE_FALSE(trie.erase(6));
    // {1, 3, 9}
    CHECK(trie.size() == 3);
    CHECK_FALSE(trie.find(5));
    CHECK(trie.next(4) == 9);
    CHECK(trie.xor_min(5) == 4);
}

TEST_CASE("32-bit bit trie", "[bit_trie]") {
    BitTrie<uint32_t, 32> trie;
    CHECK(trie.empty());
    REQUIRE(trie.insert(0xabcdef01));
    REQUIRE(trie.insert(0xabcdef02));
    REQUIRE(trie.insert(0xfedcba03));
    REQUIRE(trie.insert(0xfedcba04));
    REQUIRE_FALSE(trie.insert(0xabcdef01));
    REQUIRE(trie.insert(0xabcd1234));
    REQUIRE(trie.insert(0xabcd4321));
    // {0xabcd1234, 0xabcd4321, 0xabcdef01, 0xabcdef02, 0xfedcba03, 0xfedcba04}
    CHECK_FALSE(trie.empty());
    CHECK(trie.size() == 6);
    CHECK(trie.find(0xabcd1234));
    CHECK(trie.find(0xabcdef01));
    CHECK_FALSE(trie.find(0xfedcba01));
    CHECK_FALSE(trie.find(0xffffffff));
    CHECK(trie.next(0xabcdabcd) == 0xabcdef01);
    CHECK(trie.next(0xabcdef02) == 0xabcdef02);
    CHECK(trie.next(0xabcdef03) == 0xfedcba03);
    CHECK_FALSE(trie.next(0xff000000));
    CHECK_FALSE(trie.prev(0xaaaaaaaa));
    CHECK(trie.prev(0xabcdabcd) == 0xabcd4321);
    CHECK(trie.prev(0xfedcba02) == 0xabcdef02);
    CHECK(trie.prev(0xfedcba03) == 0xfedcba03);
    CHECK(trie.xor_min(0xabcdef42) == 0x40);
    CHECK(trie.xor_min(0xabcd0000) == 0x1234);
    CHECK(trie.xor_min(0xdeadbeef) == 0x207104eb);
    REQUIRE(trie.erase(0xabcd4321));
    REQUIRE(trie.erase(0xfedcba04));
    REQUIRE_FALSE(trie.erase(0xabcdef03));
    // {0xabcd1234, 0xabcdef01, 0xabcdef02, 0xfedcba03}
    CHECK(trie.size() == 4);
    CHECK_FALSE(trie.find(0xfedcba04));
    CHECK(trie.next(0xabcd2345) == 0xabcdef01);
    CHECK(trie.xor_min(0xfedcba00) == 0x03);
}