cmake_minimum_required(VERSION 3.17)
project(bloomfilter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -O3 -lgtest ")

add_executable(bloomfilter main.cpp include/simple_bloom_filter.h tests/simple_bloom_filter_tests.h include/helpers.h include/string_bloom_filter.h tests/string_bloom_filter_tests.h)