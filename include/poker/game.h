#pragma once
#include "core/datatypes.h"
#include "core/property.h"
#include "games/game.h"
#include "poker/bank.h"
#include "poker/coin.h"
#include "poker/deck.h"
#include "poker/player.h"

namespace poker {

class game_poker: public games::game {
public:
    bot::property<class bank> bank;
    bot::property<class deck> deck;

    game_poker(const std::vector<bot::user_ptr>& users,
               const bank::init_vals_t& bank_vals);

    auto add_player(const bot::user_ptr user) -> bool override;
};

game_poker::game_poker(const std::vector<bot::user_ptr>& users,
                       const bank::init_vals_t& bank_vals):
    games::game(),
    bank(bank_vals) {}

auto game_poker::add_player(const bot::user_ptr user) -> bool {
    if(state() == state::playing) {
        return false;
    }
    players().emplace_back(new player_poker(user));
    return true;
}

}; // namespace poker