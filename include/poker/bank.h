#pragma once
#include "core/property.h"
#include "poker/coin.h"

#include <memory>
#include <vector>

namespace poker {

class bank {
public:
    using coin_ptr = std::unique_ptr<coin>;
    using coins_t  = std::vector<coin_ptr>;

    bank(const size_t& size = 0);
    bot::property<coins_t> coins;

    auto get_coins(const std::size_t& count) -> coins_t;
    void add_coins(coins_t& coins);
};

bank::bank(const size_t& size) {
    auto& vec = coins.get();
    vec.reserve(size);
    for(auto i = 0; i < size; i++) {
        vec.emplace_back(new coin(1));
    }
}

auto bank::get_coins(const std::size_t& count) -> bank::coins_t {
    auto& coins = this->coins();
    auto i      = count;
    coins_t result;
    if(count > coins.size()) {
        auto mes = "Attempt to get " + std::to_string(count) +
                   "coins from a bank with size " + std::to_string(count);
        throw std::runtime_error(mes);
    }
    do {
        auto it = coins.begin() + i - count;
        auto c  = std::move(*it);
        result.emplace_back(std::move(c));
        i--;
    } while(i != 0);
    coins.erase(coins.begin(), coins.begin() + count);
    return result;
}
void bank::add_coins(bank::coins_t& coins) {
    this->coins().reserve(this->coins().size() + coins.size());
    for(auto& c: coins) {
        this->coins().emplace_back(std::move(c));
    }
    coins.clear();
    coins.shrink_to_fit();
}

}; // namespace poker