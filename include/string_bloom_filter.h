//
// Created by fail on 6/30/21.
//

#ifndef BLOOMFILTER_STRING_BLOOM_FILTER_H
#define BLOOMFILTER_STRING_BLOOM_FILTER_H

#include <stdexcept>
#include <bitset>
#include <vector>
#include <functional>
#include <cmath>
#include <string>

static uint64_t rotl64 (std::uint64_t x, std::uint8_t r) {
    return (x << r) | (x >> (64 - r));
}

static uint64_t fmix64 (std::uint64_t k ) {
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccd;
    k ^= k >> 33;
    k *= 0xc4ceb9fe1a85ec53;
    k ^= k >> 33;

    return k;
}

//MurmurHash3, adapted from https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp
static std::pair<std::uint64_t, std::uint64_t> mm_hash(const std::string& key, const uint32_t seed) {
    const uint8_t* data = reinterpret_cast<const uint8_t*>(key.data());
    const int num_blocks = key.length() / 16;

    uint64_t h1 = seed;
    uint64_t h2 = seed;

    const uint64_t c1 = 0x87c37b91114253d5;
    const uint64_t c2 = 0x4cf5ad432745937f;

    const uint64_t* blocks = reinterpret_cast<const uint64_t*>(data);

    for(int i = 0; i < num_blocks; i++) {
        uint64_t k1 = blocks[2 * i];
        uint64_t k2 = blocks[2 * i + 1];

        k1 *= c1;
        k1 = rotl64(k1,31);
        k1 *= c2;
        h1 ^= k1;

        h1 = rotl64(h1,27);
        h1 += h2;
        h1 = h1 * 5 + 0x52dce729;

        k2 *= c2;
        k2 = rotl64(k2,33);
        k2 *= c1;
        h2 ^= k2;

        h2 = rotl64(h2,31);
        h2 += h1;
        h2 = h2 * 5 + 0x38495ab5;
    }

    const uint8_t* tail = reinterpret_cast<const uint8_t*>(data + num_blocks * 16);

    uint64_t k1 = 0;
    uint64_t k2 = 0;

    switch(key.length() & 15) {
        case 15:
            k2 ^= static_cast<uint64_t>(tail[14]) << 48;
        case 14:
            k2 ^= static_cast<uint64_t>(tail[13]) << 40;
        case 13:
            k2 ^= static_cast<uint64_t>(tail[12]) << 32;
        case 12:
            k2 ^= static_cast<uint64_t>(tail[11]) << 24;
        case 11:
            k2 ^= static_cast<uint64_t>(tail[10]) << 16;
        case 10:
            k2 ^= static_cast<uint64_t>(tail[9]) << 8;
        case  9:
            k2 ^= static_cast<uint64_t>(tail[8]) << 0;
            k2 *= c2;
            k2 = rotl64(k2,33);
            k2 *= c1;
            h2 ^= k2;
        case  8:
            k1 ^= static_cast<uint64_t>(tail[7]) << 56;
        case  7:
            k1 ^= static_cast<uint64_t>(tail[6]) << 48;
        case  6:
            k1 ^= static_cast<uint64_t>(tail[5]) << 40;
        case  5:
            k1 ^= static_cast<uint64_t>(tail[4]) << 32;
        case  4:
            k1 ^= static_cast<uint64_t>(tail[3]) << 24;
        case  3:
            k1 ^= static_cast<uint64_t>(tail[2]) << 16;
        case  2:
            k1 ^= static_cast<uint64_t>(tail[1]) << 8;
        case  1:
            k1 ^= static_cast<uint64_t>(tail[0]) << 0;
            k1 *= c1;
            k1 = rotl64(k1,31);
            k1 *= c2;
            h1 ^= k1;
    };

    h1 ^= key.length();
    h2 ^= key.length();

    h1 += h2;
    h2 += h1;

    h1 = fmix64(h1);
    h2 = fmix64(h2);

    h1 += h2;
    h2 += h1;

    return {h1, h2};
}

namespace bloom {

    template<std::size_t Size, std::size_t HashCount>
    class [[maybe_unused]] string_bloom_filter {
    public:
        string_bloom_filter() = default;
        string_bloom_filter(const string_bloom_filter& other) noexcept = default;
        string_bloom_filter& operator=(const string_bloom_filter& other) noexcept = default;
        string_bloom_filter& operator=(string_bloom_filter&& other) noexcept = default;

        ~string_bloom_filter() = default;

        [[maybe_unused]] constexpr void clear() noexcept {
            filter.reset();
        }

        [[maybe_unused]] [[nodiscard]] constexpr bool empty() const noexcept {
            return filter.none();
        }

        [[maybe_unused]] void insert(const std::string& elem) {
            auto p = mm_hash(elem, 0xbc9f1d34);

            for(std::size_t i = 0; i < HashCount; ++i) {
                filter[(p.first + p.second * i) % filter.size()] = 1;
            }
        }

        //Checks if filter contains an element. May return false positives.
        [[maybe_unused]] [[nodiscard]] bool contains(const std::string& elem) const {
            auto p = mm_hash(elem, 0xbc9f1d34);

            for(std::size_t i = 0; i < HashCount; ++i) {
                if(!filter[(p.first + p.second * i) % filter.size()]) return false;
            }

            return true;
        }

        [[maybe_unused]] [[nodiscard]] constexpr std::bitset<Size> getFilter() const noexcept {
            return filter;
        }

        [[maybe_unused]] [[nodiscard]] constexpr string_bloom_filter merge(const string_bloom_filter& other) const noexcept {
            std::bitset<Size> output = other.filter;
            output |= filter;

            return string_bloom_filter(output);
        }

        [[nodiscard]] constexpr string_bloom_filter operator|(const string_bloom_filter& other) const {
            return this->merge(other);
        }

        constexpr string_bloom_filter& operator|=(const string_bloom_filter& other) noexcept {
            filter |= other.filter;

            return *this;
        }

        [[maybe_unused]] [[nodiscard]] constexpr string_bloom_filter intersect(const string_bloom_filter& other) const noexcept {
            std::bitset<Size> output = other.filter;
            output &= filter;

            return string_bloom_filter(output);
        }

        [[nodiscard]] constexpr string_bloom_filter operator&(const string_bloom_filter& other) const noexcept {
            return this->intersect(other);
        }

        constexpr string_bloom_filter& operator&=(const string_bloom_filter& other) noexcept {
            filter &= other.filter;

            return *this;
        }

        [[nodiscard]] constexpr bool operator==(const string_bloom_filter& other) const noexcept {
            return filter == other.filter;
        }

        [[nodiscard]] constexpr bool operator!=(const string_bloom_filter& other) const noexcept {
            return filter != other.filter;
        }
    private:
        [[maybe_unused]] explicit string_bloom_filter(const std::bitset<Size>& vd) noexcept : filter(vd) {};

        std::bitset<Size> filter;
    };
}

#endif //BLOOMFILTER_STRING_BLOOM_FILTER_H
