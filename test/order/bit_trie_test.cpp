#include <vector>
#include "catch2/catch.hpp"
#include "cplib/order/bit_trie.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Small bit trie", "[bit_trie]") {
    BitTrie<uint8_t, 4> trie;
    REQUIRE(trie.empty());
    REQUIRE(trie.insert(3));
    REQUIRE(trie.insert(1));
    REQUIRE(trie.insert(4));
    REQUIRE(!trie.insert(1));
    REQUIRE(trie.insert(5));
    REQUIRE(trie.insert(9));
    // {1, 3, 4, 5, 9}
    REQUIRE(!trie.empty());
    REQUIRE(trie.size() == 5);
    REQUIRE(trie.find(5));
    REQUIRE(trie.find(9));
    REQUIRE(!trie.find(2));
    REQUIRE(!trie.find(15));
    REQUIRE(trie.next(3) == 3);
    REQUIRE(trie.next(6) == 9);
    REQUIRE(trie.next(10) == std::nullopt);
    REQUIRE(trie.prev(0) == std::nullopt);
    REQUIRE(trie.prev(2) == 1);
    REQUIRE(trie.prev(4) == 4);
    REQUIRE(trie.xor_min(3) == 0);
    REQUIRE(trie.xor_min(7) == 2);
    REQUIRE(trie.xor_min(14) == 7);
    REQUIRE(trie.erase(4));
    REQUIRE(trie.erase(5));
    REQUIRE(!trie.erase(6));
    // {1, 3, 9}
    REQUIRE(trie.size() == 3);
    REQUIRE(!trie.find(5));
    REQUIRE(trie.next(4) == 9);
    REQUIRE(trie.xor_min(5) == 4);
}

TEST_CASE("32-bit bit trie", "[bit_trie]") {
    BitTrie<uint32_t, 32> trie;
    REQUIRE(trie.empty());
    REQUIRE(trie.insert(0xabcdef01));
    REQUIRE(trie.insert(0xabcdef02));
    REQUIRE(trie.insert(0xfedcba03));
    REQUIRE(trie.insert(0xfedcba04));
    REQUIRE(!trie.insert(0xabcdef01));
    REQUIRE(trie.insert(0xabcd1234));
    REQUIRE(trie.insert(0xabcd4321));
    // {0xabcd1234, 0xabcd4321, 0xabcdef01, 0xabcdef02, 0xfedcba03, 0xfedcba04}
    REQUIRE(!trie.empty());
    REQUIRE(trie.size() == 6);
    REQUIRE(trie.find(0xabcd1234));
    REQUIRE(trie.find(0xabcdef01));
    REQUIRE(!trie.find(0xfedcba01));
    REQUIRE(!trie.find(0xffffffff));
    REQUIRE(trie.next(0xabcdabcd) == 0xabcdef01);
    REQUIRE(trie.next(0xabcdef02) == 0xabcdef02);
    REQUIRE(trie.next(0xabcdef03) == 0xfedcba03);
    REQUIRE(trie.next(0xff000000) == std::nullopt);
    REQUIRE(trie.prev(0xaaaaaaaa) == std::nullopt);
    REQUIRE(trie.prev(0xabcdabcd) == 0xabcd4321);
    REQUIRE(trie.prev(0xfedcba02) == 0xabcdef02);
    REQUIRE(trie.prev(0xfedcba03) == 0xfedcba03);
    REQUIRE(trie.xor_min(0xabcdef42) == 0x40);
    REQUIRE(trie.xor_min(0xabcd0000) == 0x1234);
    REQUIRE(trie.xor_min(0xdeadbeef) == 0x207104eb);
    REQUIRE(trie.erase(0xabcd4321));
    REQUIRE(trie.erase(0xfedcba04));
    REQUIRE(!trie.erase(0xabcdef03));
    // {0xabcd1234, 0xabcdef01, 0xabcdef02, 0xfedcba03}
    REQUIRE(trie.size() == 4);
    REQUIRE(!trie.find(0xfedcba04));
    REQUIRE(trie.next(0xabcd2345) == 0xabcdef01);
    REQUIRE(trie.xor_min(0xfedcba00) == 0x03);
}