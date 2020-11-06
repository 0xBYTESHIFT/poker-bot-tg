#pragma once
#include "core/datatypes.h"
#include "core/property.h"
#include "core/user.h"
#include "games/player.h"
#include "poker/card.h"
#include "poker/coin.h"

#include <memory>

namespace poker {

class player_poker: public games::player {
public:
    using card_ptr = std::unique_ptr<class card>;
    using cards_t  = std::vector<card_ptr>;
    using coin_ptr = std::unique_ptr<class coin>;
    using coins_t  = std::vector<coin_ptr>;

    player_poker(bot::user_ptr user);

    void clear_cards();
    void add_card(card_ptr&& c);

    bot::property<cards_t> cards;
    bot::property<coins_t> coins;

protected:
};

player_poker::player_poker(bot::user_ptr user): games::player(user) {}

void player_poker::clear_cards() {
    this->cards().clear();
}
void player_poker::add_card(card_ptr&& c) {
    this->cards().emplace_back(std::move(c));
}
}; // namespace poker