#pragma once
#include "poker/card.h"
#include "poker/kinds.h"

#include <algorithm>
#include <memory>
#include <random>
#include <vector>

namespace poker {

class deck {
public:
    using deck_t = std::vector<card>;

    deck();

    void refill();
    void shuffle();
    auto get_cards() -> deck_t&;
    auto get_cards() const -> const deck_t&;
    auto get_card() -> card;
    auto peek_card() const -> const card&;

protected:
    std::mt19937 gen;
    std::random_device rd;
    deck_t m_cards;
};

deck::deck() {
    this->gen = std::mt19937(rd());
    refill();
    shuffle();
}
void deck::refill() {
    m_cards.clear();

    const unsigned size = 13 * 4;
    m_cards.reserve(size);
    for(unsigned i = 0; i < size; ++i) {
        const auto kind_index = (i / 13);
        auto& kind            = (kind_index == 0) ? hearts :
                                (kind_index == 1) ? tiles :
                                (kind_index == 2) ? clovers :
                                                    pikes;
        auto value            = (i % 13) + 2;
        m_cards.emplace_back(value, kind);
    }
}
void deck::shuffle() {
    std::shuffle(m_cards.begin(), m_cards.end(), this->gen);
}

auto deck::get_cards() -> deck_t& {
    return m_cards;
}
auto deck::get_cards() const -> const deck_t& {
    return m_cards;
}
auto deck::get_card() -> card {
    auto beg  = m_cards.begin();
    auto card = std::move(*beg);
    m_cards.erase(beg);
    return card;
}
auto deck::peek_card() const -> const card& {
    return m_cards.front();
}

}; // namespace poker