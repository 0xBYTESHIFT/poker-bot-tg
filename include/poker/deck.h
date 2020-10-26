#pragma once
#include <vector>
#include <memory>
#include "poker/card.h"
#include "poker/kinds.h"

namespace poker{

class deck{
public:
    using deck_t = std::vector<std::unique_ptr<card>>;

    deck();

    void refill();
    void shuffle();
    auto get_cards()const -> const deck_t&;
protected:
    deck_t m_cards;
};

deck::deck(){
    refill();
}
void deck::refill(){
    m_cards.clear();

    const auto size = 13*4;
    m_cards.reserve(size);
    for(auto i=0; i<size; ++i){
        const auto kind_index = (i/13);
        auto &kind = (kind_index == 0)? hearts:
            (kind_index == 1)? tiles:
            (kind_index == 2)? clovers:
            pikes;
        auto value = (i%13); 
        m_cards.emplace_back(new card(value, kind));
    }
}
void deck::shuffle(){
    std::random_shuffle(m_cards.begin(), m_cards.end());
}

auto deck::get_cards()const -> const deck_t&{
    return m_cards;
}

};