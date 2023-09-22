#include "cplib/range/bit_dict.hpp"

#include "catch2/catch_test_macros.hpp"
using namespace std;
using namespace cplib;

TEST_CASE("Bit dict with small array", "[bit_dict]") {
  BitDict bd(20);
  for (int p : {2, 3, 5, 7, 11, 13, 17, 19}) {
    bd.set(p);
  }
  bd.build();
  CHECK(bd.size() == 20);
  CHECK(bd.ones() == 8);
  CHECK(bd.zeros() == 12);
  CHECK(bd.get(7) == true);
  CHECK(bd.get(9) == false);
  CHECK(bd.rank1(0) == 0);
  CHECK(bd.rank1(8) == 4);
  CHECK(bd.rank1(13) == 5);
  CHECK(bd.rank1(20) == 8);
  CHECK(bd.rank0(0) == 0);
  CHECK(bd.rank0(4) == 2);
  CHECK(bd.rank0(11) == 7);
  CHECK(bd.rank0(20) == 12);
  CHECK(bd.rank_to_child(10, false) == 6);
  CHECK(bd.rank_to_child(10, true) == 16);
}

TEST_CASE("Bit dict with large array", "[bit_dict]") {
  BitDict bd(12345);
  for (int i = 0; i < 12345; i += 10) {
    bd.set(i);
  }
  bd.build();
  CHECK(bd.size() == 12345);
  CHECK(bd.ones() == 1235);
  CHECK(bd.zeros() == 11110);
  CHECK(bd.get(1010) == true);
  CHECK(bd.get(1919) == false);
  CHECK(bd.rank1(0) == 0);
  CHECK(bd.rank1(1000) == 100);
  CHECK(bd.rank1(9999) == 1000);
  CHECK(bd.rank1(12345) == 1235);
  CHECK(bd.rank0(0) == 0);
  CHECK(bd.rank0(42) == 37);
  CHECK(bd.rank0(11111) == 9999);
  CHECK(bd.rank0(12345) == 11110);
  CHECK(bd.rank_to_child(5000, false) == 4500);
  CHECK(bd.rank_to_child(5000, true) == 11610);
}

TEST_CASE("Bit dict with power of 2 array", "[bit_dict]") {
  BitDict bd(65536);
  for (int i = 1; i <= 65536; i *= 2) {
    bd.set(i - 1);
  }
  bd.build();
  CHECK(bd.size() == 65536);
  CHECK(bd.ones() == 17);
  CHECK(bd.zeros() == 65519);
  CHECK(bd.get(1023) == true);
  CHECK(bd.get(1234) == false);
  CHECK(bd.rank1(0) == 0);
  CHECK(bd.rank1(1000) == 10);
  CHECK(bd.rank1(16384) == 15);
  CHECK(bd.rank1(65536) == 17);
  CHECK(bd.rank0(0) == 0);
  CHECK(bd.rank0(42) == 36);
  CHECK(bd.rank0(4095) == 4083);
  CHECK(bd.rank0(65536) == 65519);
  CHECK(bd.rank_to_child(10000, false) == 9986);
  CHECK(bd.rank_to_child(10000, true) == 65533);
}

TEST_CASE("Bit dict with mostly 1's", "[bit_dict]") {
  BitDict bd(12345);
  for (int i = 0; i < 12345; i++) {
    if (i % 100 != 99) {
      bd.set(i);
    }
  }
  bd.build();
  CHECK(bd.size() == 12345);
  CHECK(bd.zeros() == 123);
  CHECK(bd.ones() == 12222);
  CHECK(bd.get(1234) == true);
  CHECK(bd.get(9999) == false);
  CHECK(bd.rank1(0) == 0);
  CHECK(bd.rank1(999) == 990);
  CHECK(bd.rank1(10000) == 9900);
  CHECK(bd.rank1(12345) == 12222);
  CHECK(bd.rank0(0) == 0);
  CHECK(bd.rank0(999) == 9);
  CHECK(bd.rank0(10000) == 100);
  CHECK(bd.rank0(12345) == 123);
  CHECK(bd.rank_to_child(10000, false) == 100);
  CHECK(bd.rank_to_child(10000, true) == 10023);
}