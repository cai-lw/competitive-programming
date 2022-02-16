#include "catch2/catch.hpp"
#include "cplib/range/bit_dict.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Bit dict with small array", "[bit_dict]") {
    BitDictBuilder builder(20);
    for (int p : {2, 3, 5, 7, 11, 13, 17, 19}) {
        builder.set(p);
    }
    BitDict bd = builder.build();
    REQUIRE(bd.size() == 20);
    REQUIRE(bd.ones() == 8);
    REQUIRE(bd.zeros() == 12);
    REQUIRE(bd[7] == true);
    REQUIRE(bd[9] == false);
    REQUIRE(bd.rank1(0) == 0);
    REQUIRE(bd.rank1(8) == 4);
    REQUIRE(bd.rank1(13) == 5);
    REQUIRE(bd.rank1(20) == 8);
    REQUIRE(bd.rank0(0) == 0);
    REQUIRE(bd.rank0(4) == 2);
    REQUIRE(bd.rank0(11) == 7);
    REQUIRE(bd.rank0(20) == 12);
    REQUIRE(bd.rank_to_child(10, false) == 6);
    REQUIRE(bd.rank_to_child(10, true) == 16);
}

TEST_CASE("Bit dict with large array", "[bit_dict]") {
    BitDictBuilder builder(12345);
    for (int i = 0; i < 12345; i += 10) {
        builder.set(i);
    }
    BitDict bd = builder.build();
    REQUIRE(bd.size() == 12345);
    REQUIRE(bd.ones() == 1235);
    REQUIRE(bd.zeros() == 11110);
    REQUIRE(bd[1010] == true);
    REQUIRE(bd[1919] == false);
    REQUIRE(bd.rank1(0) == 0);
    REQUIRE(bd.rank1(1000) == 100);
    REQUIRE(bd.rank1(9999) == 1000);
    REQUIRE(bd.rank1(12345) == 1235);
    REQUIRE(bd.rank0(0) == 0);
    REQUIRE(bd.rank0(42) == 37);
    REQUIRE(bd.rank0(11111) == 9999);
    REQUIRE(bd.rank0(12345) == 11110);
    REQUIRE(bd.rank_to_child(5000, false) == 4500);
    REQUIRE(bd.rank_to_child(5000, true) == 11610);
}

TEST_CASE("Bit dict with power of 2 array", "[bit_dict]") {
    BitDictBuilder builder(65536);
    for (int i = 1; i <= 65536; i *= 2) {
        builder.set(i - 1);
    }
    BitDict bd = builder.build();
    REQUIRE(bd.size() == 65536);
    REQUIRE(bd.ones() == 17);
    REQUIRE(bd.zeros() == 65519);
    REQUIRE(bd[1023] == true);
    REQUIRE(bd[1234] == false);
    REQUIRE(bd.rank1(0) == 0);
    REQUIRE(bd.rank1(1000) == 10);
    REQUIRE(bd.rank1(16384) == 15);
    REQUIRE(bd.rank1(65536) == 17);
    REQUIRE(bd.rank0(0) == 0);
    REQUIRE(bd.rank0(42) == 36);
    REQUIRE(bd.rank0(4095) == 4083);
    REQUIRE(bd.rank0(65536) == 65519);
    REQUIRE(bd.rank_to_child(10000, false) == 9986);
    REQUIRE(bd.rank_to_child(10000, true) == 65533);
}