#pragma once
#include <cstddef>
#include <utility>

namespace poker{

struct coin{
    const std::size_t value;

    coin(const std::size_t &value);
    coin(const coin &rhs);
    coin(coin &&rhs);
};

coin::coin(const std::size_t &value)
    :value(value)
{}
coin::coin(const coin &rhs)
    :value(rhs.value)
{}
coin::coin(coin &&rhs)
    :value(std::move(rhs.value))
{}

};