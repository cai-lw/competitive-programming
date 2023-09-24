cmake_minimum_required(VERSION 3.8.0)
project(competitive-programming VERSION 0.0.1)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Catch2 EXCLUDE_FROM_ALL)
add_subdirectory(test)