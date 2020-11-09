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
    void init_game();

private:
    void p_fill_hand(game::player_ptr pl);
};

game_poker::game_poker(const std::vector<bot::user_ptr>& users,
                       const bank::init_vals_t& bank_vals):
    games::game(),
    bank(bank_vals) {
    this->state() = state::ended;
    for(auto& u: users) {
        add_player(u);
    }
}

auto game_poker::add_player(const bot::user_ptr user) -> bool {
    auto present = bot::utils::contains_if(
        players(), [&](auto pl) { return pl->user == user; });
    if(state() == state::playing || present) {
        return false;
    }
    players().emplace_back(new player_poker(user));
    return true;
}

void game_poker::init_game() {
    deck().refill();
    deck().shuffle();
    for(auto& pl: players()) {
        p_fill_hand(pl);
    }
}
void game_poker::p_fill_hand(game::player_ptr pl) {
    auto p     = std::dynamic_pointer_cast<player_poker>(pl);
    auto card1 = deck().get_card();
    auto card2 = deck().get_card();
    std::string mes =
        std::to_string(card1->value) + " " + card1->kind.name + " ";
    mes += std::to_string(card2->value) + " " + card2->kind.name + " ";
    p->add_card(std::move(card1));
    p->add_card(std::move(card2));
    p->mes_to_send.emplace(std::move(mes));
}

}; // namespace poker