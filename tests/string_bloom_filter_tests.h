//
// Created by fail on 6/30/21.
//

#ifndef BLOOMFILTER_STRING_BLOOM_FILTER_TESTS_H
#define BLOOMFILTER_STRING_BLOOM_FILTER_TESTS_H

#include "gtest/gtest.h"
#include "../include/string_bloom_filter.h"

TEST(string_bloom_filter, insert_contains) {
    bloom::string_bloom_filter<128, 5> filter;

    filter.insert("A quick brown fox jumped over the lazy dogs");
    filter.insert("Hello, world!");
    filter.insert("A sample string.");

    ASSERT_FALSE(filter.contains("Different string"));
    ASSERT_FALSE(filter.contains("Yet another different string"));

    ASSERT_TRUE(filter.contains("Hello, world!"));
    ASSERT_TRUE(filter.contains("A sample string."));

    auto filter_copy = filter;

    ASSERT_FALSE(filter_copy.contains("Goodbye"));
    ASSERT_TRUE(filter_copy.contains("A quick brown fox jumped over the lazy dogs"));
}

TEST(string_bloom_filter, helpers) {
    bloom::string_bloom_filter<128, 5> filter;
    bloom::string_bloom_filter<128, 5> other_filter;
    //Both filter are empty.
    ASSERT_TRUE(filter.empty());

    //First filter contains "A quick ...", second one is empty.
    filter.insert("A quick brown fox jumped over the lazy dogs");

    ASSERT_NE(filter, other_filter);

    //First filter contains "A quick ...", second one contains "Hello, world".
    other_filter.insert("Hello, world!");
    ASSERT_NE(filter, other_filter);

    //First filter contains both strings, second one contains "Hello, world".
    filter |= other_filter;
    ASSERT_TRUE(filter.contains("Hello, world!"));
    ASSERT_TRUE(filter.contains("A quick brown fox jumped over the lazy dogs"));

    //First filter contains both strings, second one contains "Hello, world".
    other_filter &= filter;
    ASSERT_TRUE(other_filter.contains("Hello, world!"));
    ASSERT_FALSE(other_filter.contains("A quick brown fox jumped over the lazy dogs"));

    //A contains both strings.
    auto a = other_filter | filter;
    ASSERT_TRUE(a.contains("Hello, world!"));
    ASSERT_TRUE(a.contains("A quick brown fox jumped over the lazy dogs"));

    //B contains "Hello, world".
    auto b = other_filter & filter;
    ASSERT_TRUE(b.contains("Hello, world!"));
    ASSERT_FALSE(b.contains("A quick brown fox jumped over the lazy dogs"));

    a.clear();
    ASSERT_TRUE(a.getFilter().none());
}

#endif //BLOOMFILTER_STRING_BLOOM_FILTER_TESTS_H
