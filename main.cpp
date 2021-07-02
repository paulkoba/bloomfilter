#include "tests/simple_bloom_filter_tests.h"
#include "tests/string_bloom_filter_tests.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}