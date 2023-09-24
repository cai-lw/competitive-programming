add_executable(run_tests
    conv/anymod_test.cpp
    conv/conv_test.cpp
    conv/multivar_test.cpp
    hash/hash_table_test.cpp
    num/discrete_log_test.cpp
    num/factor_test.cpp
    num/gcd_test.cpp
    num/modint_test.cpp
    num/prime_test.cpp
    num/primitive_root_test.cpp
    num/sqrt_test.cpp
    order/bit_trie_test.cpp
    order/dary_heap_test.cpp
    order/pairing_heap_test.cpp
    range/bit_dict_test.cpp
    range/rmq_test.cpp
    range/wavelet_array_test.cpp
)

target_include_directories(run_tests PRIVATE "${CMAKE_SOURCE_DIR}/src")

set_target_properties(run_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_link_libraries(run_tests PRIVATE Catch2WithMain)

include("${CMAKE_SOURCE_DIR}/Catch2/extras/Catch.cmake")
catch_discover_tests(run_tests)