A small header-only implementation of a bloom-filter I wrote as an exercise. Sample usage:

```CPP
#include "simple_bloom_filter.h"
#include "string_bloom_filter.h"

void test() {
    bloom::string_bloom_filter<65536, 7> filter; // Creates a filter with a size of 65536 bits and 7 hash functions.
    filter.insert("Hello, world");
    bool may_contain_hello_world = filter.contains("Hello, world"); // True.
}
```
