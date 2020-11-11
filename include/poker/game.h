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
    using card_ptr = deck::card_ptr;
    bot::property<class bank> bank;
    bot::property<class deck> deck;
    bot::property<std::vector<card_ptr>> table;

    game_poker(const std::vector<bot::user_ptr>& users,
               const std::size_t& bank_size);

    auto add_player(const bot::user_ptr user) -> bool override;
    void handle_exit(const game::player_ptr pl) override;
    void init_game();

private:
    std::size_t p_place; /**< index of a current player*/
    void p_fill_hand(game::player_ptr pl);
    auto p_render_game_state() const -> std::string;
    auto p_render_card(const card_ptr& c) const -> std::string;
    auto p_render_coins(const bank::coins_t& c) const -> std::string;
    void p_send_state(const std::string& game_state, player_ptr pl);
};

game_poker::game_poker(const std::vector<bot::user_ptr>& users,
                       const std::size_t& bank_size):
    games::game(),
    bank(bank_size) {
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
    const auto state = p_render_game_state();
    for(auto& pl: players()) {
        p_fill_hand(pl);
        p_send_state(state, pl);
    }
}
void game_poker::p_fill_hand(game::player_ptr pl) {
    auto p     = std::dynamic_pointer_cast<player_poker>(pl);
    auto card1 = deck().get_card();
    auto card2 = deck().get_card();

    p->add_card(std::move(card1));
    p->add_card(std::move(card2));
}
auto game_poker::p_render_game_state() const -> std::string {
    std::string mes;
    mes = "Bank: " + p_render_coins(bank().coins());
    mes += "\nTable: ";
    for(auto& card: table()) {
        mes += p_render_card(card) + " ";
    }
    return mes;
}
auto game_poker::p_render_card(const card_ptr& c) const -> std::string {
    std::string mes = std::to_string(c->value) + " " + c->kind.name;
    return mes;
}
auto game_poker::p_render_coins(const bank::coins_t& c) const -> std::string {
    std::string mes = std::to_string(c.size());
    return mes;
}
void game_poker::p_send_state(const std::string& game_state, player_ptr pl) {
    auto mes = p_render_game_state();
    mes += "\nYour bank:";
    auto cast = std::dynamic_pointer_cast<player_poker>(pl);
    mes += p_render_coins(cast->bank().coins());
    mes += "\nHand:" + p_render_card(cast->cards().at(0)) + " " +
           p_render_card(cast->cards().at(1));
    pl->mes_to_send.emplace(mes);
}

}; // namespace poker