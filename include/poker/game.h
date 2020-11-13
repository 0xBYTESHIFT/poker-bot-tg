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
    using player_ptr = std::shared_ptr<player_poker>;
    using card_ptr   = deck::card_ptr;
    bot::property<class bank> bank;
    bot::property<class deck> cards;
    bot::property<std::vector<card_ptr>> table;

    game_poker(const std::vector<bot::user_ptr>& users,
               const std::size_t& bank_size, const std::size_t& blind_bet);

    auto add_player(const bot::user_ptr user) -> bool override;
    void handle_exit(const game::player_ptr pl) override;

    void init_game();
    void handle_bet(bot::user_ptr user, const std::size_t& size);
    void handle_fold(bot::user_ptr user);

private:
    std::size_t p_place;           /**< index of a current player */
    std::size_t p_last_bet;        /**< last bet to keep track */
    std::size_t p_big_blind_place; /**< index of a big blind */
    std::size_t p_big_blind_bet;   /**< amount of required big blind */

    auto p_user_to_player(const bot::user_ptr u) -> game_poker::player_ptr;
    void p_advance_place();
    auto p_get_blind_big() -> game_poker::player_ptr;
    auto p_get_blind_small() -> game_poker::player_ptr;
    void p_fill_hand(game::player_ptr pl);
    auto p_render_game_state() const -> std::string;
    auto p_render_card(const card_ptr& c) const -> std::string;
    auto p_render_coins(const bank::coins_t& c) const -> std::string;
    void p_send_state(const std::string& game_state, game::player_ptr pl);
};

game_poker::game_poker(const std::vector<bot::user_ptr>& users,
                       const std::size_t& bank_size,
                       const std::size_t& blind_bet):
    games::game(),
    bank(bank_size) {
    this->state()     = state::ended;
    p_place           = 0;
    p_last_bet        = 0;
    p_big_blind_place = 0;
    p_big_blind_bet   = blind_bet;
    for(auto& u: users) {
        add_player(u);
    }
}

auto game_poker::add_player(const bot::user_ptr user) -> bool {
    auto pl = p_user_to_player(user);
    if(state() == state::playing || pl) {
        lgr << "user " << bot::utils::get_desc_log(user)
            << " can't join poker game\n";
        return false;
    }
    players().emplace_back(new player_poker(user));
    lgr << "user " << bot::utils::get_desc_log(user) << " joined poker game\n";
    return true;
}

void game_poker::handle_exit(const game::player_ptr pl) {
    auto cast = std::dynamic_pointer_cast<const poker::player_poker>(pl);
    if(!cast) {
        throw std::runtime_error("wrong player class in the poker game");
    }
    auto i = bot::utils::index(players(), pl);
    if(i == (size_t)-1) {
        throw std::runtime_error(
            "player in the poker game's exit handler is not present");
    }
    if(i < p_place) {
        p_place--; // -1 because the player will be deleted later
    }
    players().erase(players().begin() + i);
    lgr << "poker: " << bot::utils::get_desc_log(pl->user())
        << " joined poker game\n";
    //TODO: handle cast->coins;
}

void game_poker::init_game() {
    cards().refill();
    cards().shuffle();
    const auto state = p_render_game_state();
    for(auto& pl: players()) {
        p_fill_hand(pl);
        p_send_state(state, pl);
    }
}
void game_poker::handle_bet(bot::user_ptr user, const std::size_t& size) {
    auto pl = p_user_to_player(user);
    if(!pl) {
        return;
    }
    if(bot::utils::index(players(), pl) != p_place) {
        pl->send("It's not your turn to make a bet");
        return;
    }
    auto cast       = std::dynamic_pointer_cast<player_poker>(pl);
    auto& coins     = cast->bank().coins();
    auto& game_bank = bank().coins();
    if(coins.size() < size) {
        auto mes = "You can't make that bet, your bank is:" +
                   std::to_string(coins.size());
        pl->send(mes);
        return;
    }
    if(size < p_last_bet) {
        auto mes = "Your bet can't be lower than " + std::to_string(p_last_bet);
        pl->send(mes);
        return;
    }
    lgr << "poker: " << bot::utils::get_desc_log(user) << " made a bet:" << size
        << "\n";
    p_last_bet = size;
    std::move(coins.begin(), coins.begin() + size,
              std::back_inserter(game_bank));
    p_advance_place();
    auto mes = pl->user()->desc() + " made a bet:" + std::to_string(size);
    for(auto& pl: players()) {
        pl->send(mes);
    }
}

auto game_poker::p_user_to_player(const bot::user_ptr u)
    -> game_poker::player_ptr {
    using namespace bot::utils;
    auto pred = [&](auto pl) { return pl->user() == u; };
    auto pl   = bot::utils::find_if(players(), pred);
    if(pl == players().end()) {
        return nullptr;
    }
    return dyn_cast<player_poker>(*pl);
}
void game_poker::p_advance_place() {
    p_place++;
    p_place %= players().size();
    players().at(p_place)->send("It's your turn.");
}
auto game_poker::p_get_blind_big() -> game_poker::player_ptr {
    using namespace bot::utils;
    auto pl_ptr = players().at(p_big_blind_place);
    return dyn_cast<player_poker>(pl_ptr);
}
auto game_poker::p_get_blind_small() -> game_poker::player_ptr {
    using namespace bot::utils;
    auto small_blind_place = (p_big_blind_place + 1) % players().size();
    auto pl_ptr            = players().at(small_blind_place);
    return dyn_cast<player_poker>(pl_ptr);
}
void game_poker::p_fill_hand(game::player_ptr pl) {
    using namespace bot::utils;
    auto p     = dyn_cast<player_poker>(pl);
    auto card1 = cards().get_card();
    auto card2 = cards().get_card();

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
void game_poker::p_send_state(const std::string& game_state,
                              game::player_ptr pl) {
    auto mes = p_render_game_state();
    mes += "\nYour bank:";
    auto cast = std::dynamic_pointer_cast<player_poker>(pl);
    mes += p_render_coins(cast->bank().coins());
    mes += "\nHand:" + p_render_card(cast->cards().at(0)) + " " +
           p_render_card(cast->cards().at(1));
    pl->send(mes);
}

}; // namespace poker