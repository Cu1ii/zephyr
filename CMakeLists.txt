cmake_minimum_required(VERSION 3.16)
project(zephyr)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g -std=c++11 -Wall")

set(CMAKE_BUILD_TYPE Debug)

set(LIB_SRC
        src/include/memory/pool_allocator.h
        src/include/math/internal_bit.hpp
        src/include/memory/allocator.h
        src/include/memory/construct.h
        src/include/math/math.h
        src/include/memory/loki_allocator.h
)

set(LIB_TEST
        tests/pool_alloc_test.cpp
        tests/math_test.cpp
)


add_executable(zephyr ${LIB_SRC} tests/test.cpp)
