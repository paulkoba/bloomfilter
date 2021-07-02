//
// Created by fail on 6/29/21.
//

#ifndef BLOOMFILTER_SIMPLE_BLOOM_FILTER_H
#define BLOOMFILTER_SIMPLE_BLOOM_FILTER_H

#include <bitset>
#include <vector>
#include <functional>
#include <cmath>

namespace bloom {

    template<typename KeyType, std::size_t Size>
    class [[maybe_unused]] simple_bloom_filter {
    public:
        simple_bloom_filter() = delete;
        simple_bloom_filter(const simple_bloom_filter& other) noexcept = default;
        simple_bloom_filter& operator=(const simple_bloom_filter& other) noexcept = default;
        simple_bloom_filter& operator=(simple_bloom_filter&& other) noexcept = default;

        ~simple_bloom_filter() = default;

        // Constructs a bloom filter with given hash functions.
        [[maybe_unused]] explicit simple_bloom_filter(const std::vector<std::function<std::size_t(KeyType)>>& hash_functions) : functions(hash_functions) {
            if(functions.size() == 0) {
                throw std::invalid_argument("Empty hash-function vector supplied.");
            }
        }

        [[maybe_unused]] constexpr void clear() noexcept {
            filter.reset();
        }

        [[maybe_unused]] [[nodiscard]] constexpr bool empty() const noexcept {
            return filter.none();
        }

        [[maybe_unused]] void insert(KeyType elem) {
            for(std::function<std::size_t(KeyType)> f : functions) {
                filter[f(elem) % filter.size()] = 1;
            }
        }

        //Checks if filter contains an element. May return false positives.
        [[maybe_unused]] [[nodiscard]] bool contains(KeyType elem) const {
            for(std::function<std::size_t(KeyType)> f : functions) {
                if(!filter[f(elem) % filter.size()]) return false;
            }

            return true;
        }

        [[maybe_unused]] [[nodiscard]] constexpr std::bitset<Size> getFilter() const noexcept {
            return filter;
        }

        [[maybe_unused]] [[nodiscard]] constexpr simple_bloom_filter merge(const simple_bloom_filter& other) const noexcept {
            std::bitset<Size> output = other.filter;
            output |= filter;

            return {output, functions};
        }

        [[nodiscard]] constexpr simple_bloom_filter operator|(const simple_bloom_filter& other) const {
            return this->merge(other);
        }

        constexpr simple_bloom_filter& operator|=(const simple_bloom_filter& other) noexcept {
            filter |= other.filter;

            return *this;
        }

        [[maybe_unused]] [[nodiscard]] constexpr simple_bloom_filter intersect(const simple_bloom_filter& other) const noexcept {
            std::bitset<Size> output = other.filter;
            output &= filter;

            return {output, functions};
        }

        [[nodiscard]] constexpr simple_bloom_filter operator&(const simple_bloom_filter& other) const noexcept {
            return this->intersect(other);
        }

        constexpr simple_bloom_filter& operator&=(const simple_bloom_filter& other) noexcept {
            filter &= other.filter;

            return *this;
        }

        [[nodiscard]] constexpr bool operator==(const simple_bloom_filter& other) const noexcept {
            return filter == other.filter;
        }

        [[nodiscard]] constexpr bool operator!=(const simple_bloom_filter& other) const noexcept {
            return filter != other.filter;
        }
    private:
        [[maybe_unused]] simple_bloom_filter(const std::bitset<Size>& vd, const std::vector<std::function<std::size_t(KeyType)>>& vf) noexcept :
            filter(vd), functions(vf) {};

        std::bitset<Size> filter;
        std::vector<std::function<std::size_t(KeyType)>> functions;
    };
}

#endif //BLOOMFILTER_SIMPLE_BLOOM_FILTER_H
