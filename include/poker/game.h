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
    void handle_exit(const game::player_ptr pl) override;
    void init_game();

private:
    std::size_t p_place; /**< index of a current player*/
    void p_fill_hand(game::player_ptr pl);
};

game_poker::game_poker(const std::vector<bot::user_ptr>& users,
                       const bank::init_vals_t& bank_vals):
    games::game(),
    bank(bank_vals) {
    this->state() = state::ended;
    p_place       = 0;
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

void game_poker::handle_exit(const game::player_ptr pl) {
    auto cast = std::dynamic_pointer_cast<const poker::player_poker>(pl);
    if(!cast) {
        throw std::runtime_error("wrong player class in the poker game");
    }
    auto it = bot::utils::find(players(), pl);
    if(it == players().end()) {
        throw std::runtime_error(
            "player in the poker game's exit handler is not present");
    }
    auto place = std::distance(players().cbegin(), it);
    if(place < p_place) {
        p_place--; // -1 because the player will be deleted later
    }
    players().erase(it);
    //TODO: handle cast->coins;
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