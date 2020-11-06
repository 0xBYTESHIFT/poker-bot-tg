#pragma once
#include "core/property.h"
#include "poker/coin.h"

#include <memory>
#include <vector>

namespace poker {

class bank {
public:
    using coin_ptr    = std::unique_ptr<coin>;
    using coins_t     = std::map<size_t, std::vector<coin_ptr>>;
    using init_vals_t = std::vector<std::pair<unsigned, unsigned>>;

    bank(const init_vals_t& values);
    bot::property<coins_t> coins;
};

bank::bank(const init_vals_t& values) {
    for(auto& pair: values) {
        auto value = pair.first;
        auto size  = pair.second;
        std::vector<coin_ptr> vec;
        vec.reserve(size);
        for(auto i = 0; i < size; i++) {
            vec.emplace_back(new coin(value));
        }
        coins().emplace(value, std::move(vec));
    }
}

}; // namespace poker