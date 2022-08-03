cmake_minimum_required(VERSION 3.16)
project(zephyr)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g -std=c++11 -Wall")

set(LIB_SRC
        ${CMAKE_SOURCE_DIR}/src/include/memory/pool_alloc.hpp
        ${CMAKE_SOURCE_DIR}/src/include/math/internal_bit.hpp
        ${CMAKE_SOURCE_DIR}/src/include/memory/allocator.h
    )
add_executable(zephyr ${LIB_SRC}
        tests/alloc_test.cpp
        tests/math_test.cpp
    )
