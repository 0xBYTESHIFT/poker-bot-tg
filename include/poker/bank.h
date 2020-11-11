#pragma once
#include "core/property.h"
#include "poker/coin.h"

#include <memory>
#include <vector>

namespace poker {

class bank {
public:
    using coin_ptr    = std::unique_ptr<coin>;
    using coins_t     = std::vector<coin_ptr>;

    bank(const size_t &size=0);
    bot::property<coins_t> coins;
};

bank::bank(const size_t& size) {
    auto &vec = coins.get();
    vec.reserve(size);
    for(auto i = 0; i < size; i++) {
        vec.emplace_back(new coin(1));
    }
}

}; // namespace poker