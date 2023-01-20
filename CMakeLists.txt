cmake_minimum_required(VERSION 3.8.0)
project(competitive-programming VERSION 0.0.1)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(run_tests
    test/run_tests.cpp
    test/range/rmq_test.cpp
    test/range/bit_dict_test.cpp
    test/range/wavelet_array_test.cpp
    test/hash/hash_table_test.cpp
    test/order/bit_trie_test.cpp
    test/order/dary_heap_test.cpp
    test/order/pairing_heap_test.cpp
    test/num/gcd_test.cpp
    test/num/mmint_test.cpp
    test/num/sqrt_test.cpp
    test/num/prime_test.cpp
    test/num/factor_test.cpp
    test/conv/conv_test.cpp
    test/conv/multivar_test.cpp
    test/conv/anymod_test.cpp
)

add_subdirectory(Catch2)
target_link_libraries(run_tests Catch2::Catch2)
target_include_directories(run_tests PRIVATE src)

include(Catch2/contrib/Catch.cmake)
catch_discover_tests(run_tests)