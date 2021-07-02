//
// Created by fail on 6/30/21.
//

#ifndef BLOOMFILTER_HELPERS_H
#define BLOOMFILTER_HELPERS_H

#include <cmath>

namespace bloom {
    [[maybe_unused]] [[nodiscard]] double false_positive_rate(std::size_t filter_size, std::size_t items_in_filter,
                                                              std::size_t hash_function_count) noexcept {
        return std::pow(1. - std::exp(-(double)hash_function_count * items_in_filter / filter_size), hash_function_count);
    }

    [[maybe_unused]] [[nodiscard]] std::size_t suggested_hash_function_count(std::size_t filter_size, std::size_t items_in_filter) noexcept  {
        return std::round(std::log(2) * filter_size / items_in_filter);
    }

    [[maybe_unused]] [[nodiscard]] std::size_t suggested_filter_size(std::size_t elements_count, double probability) noexcept {
        // 1 / ln(2 ^^ ln(2))
        static const double multiplier = 2.081368981005607797869581603734991425064047155646081031626558440;

        return std::ceil(-std::log(probability) * elements_count) * multiplier;
    }
}

#endif //BLOOMFILTER_HELPERS_H
