#pragma once
#include <cstddef>
#include <utility>

namespace poker {

struct coin {
    const std::size_t value;

    coin(std::size_t value);
    coin(const coin& rhs) = delete;
    coin(coin&& rhs);
};

coin::coin(std::size_t value): value(value) { }
coin::coin(coin&& rhs): value(std::move(rhs.value)) { }

}; // namespace poker