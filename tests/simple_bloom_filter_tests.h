//
// Created by fail on 6/29/21.
//

#include "gtest/gtest.h"
#include "../include/simple_bloom_filter.h"

#ifndef BLOOMFILTER_SIMPLE_BLOOM_FILTER_TESTS_H
#define BLOOMFILTER_SIMPLE_BLOOM_FILTER_TESTS_H

std::size_t totally_hash_function(uint64_t a) {
    a = (a ^ (a >> 31U) ^ (a >> 62U)) * 0x319642b2d24d8ec3;
    a = (a ^ (a >> 27U) ^ (a >> 54U)) * 0x96de1b173f119089;
    a = a ^ (a >> 30U) ^ (a >> 60U);
    return a;
}

std::size_t totally_hash_function_2(uint64_t a) {
    a = (a ^ (a >> 30U)) * 0xbf58476d1ce4e5b9;
    a = (a ^ (a >> 27U)) * 0x94d049bb133111eb;
    a = a ^ (a >> 31U);
    return a;
}

std::size_t totally_hash_function_3(uint64_t a) {
    return (a + 1000000009ULL) * 18446744073709551557ULL;
}

TEST(simple_bloom_filter, insert_contains) {
    bloom::simple_bloom_filter<uint64_t, 128> filter({totally_hash_function, totally_hash_function_2, totally_hash_function_3});

    filter.insert(5);
    filter.insert(10);
    filter.insert(15);

    ASSERT_FALSE(filter.contains(4));
    ASSERT_FALSE(filter.contains(42));

    ASSERT_TRUE(filter.contains(5));
    ASSERT_TRUE(filter.contains(10));

    auto filter_copy = filter;

    ASSERT_FALSE(filter_copy.contains(99));
    ASSERT_TRUE(filter_copy.contains(15));
}

TEST(simple_bloom_filter, helpers) {
    bloom::simple_bloom_filter<uint64_t, 128> filter({totally_hash_function, totally_hash_function, totally_hash_function_3});
    bloom::simple_bloom_filter<uint64_t, 128> other_filter({totally_hash_function, totally_hash_function, totally_hash_function_3});

    ASSERT_TRUE(filter.empty());

    filter.insert(5);
    ASSERT_NE(filter, other_filter);

    other_filter.insert(6);
    ASSERT_NE(filter, other_filter);

    filter |= other_filter;
    ASSERT_TRUE(filter.contains(5));
    ASSERT_TRUE(filter.contains(6));

    other_filter &= filter;
    ASSERT_TRUE(other_filter.contains(6));
    ASSERT_FALSE(other_filter.contains(5));

    auto a = other_filter | filter;
    ASSERT_TRUE(a.contains(5));
    ASSERT_TRUE(a.contains(6));

    auto b = other_filter & filter;
    ASSERT_TRUE(b.contains(6));
    ASSERT_FALSE(b.contains(5));

    a.clear();
    ASSERT_TRUE(a.getFilter().none());
}

#endif //BLOOMFILTER_SIMPLE_BLOOM_FILTER_TESTS_H
