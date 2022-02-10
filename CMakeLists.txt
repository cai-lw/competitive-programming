cmake_minimum_required(VERSION 3.8.0)
project(competitive-programming VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH Catch2/contrib)

add_executable(run_tests
    test/run_tests.cpp
    test/range/rmq_test.cpp
)

add_subdirectory(Catch2)
target_link_libraries(run_tests Catch2::Catch2)
target_include_directories(run_tests PRIVATE src)

include(Catch)
catch_discover_tests(run_tests)