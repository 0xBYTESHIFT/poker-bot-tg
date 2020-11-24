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
    using player_ptr =
        std::shared_ptr<player_poker>; /**< Define for poker player ptr */
    using card_ptr = deck::card_ptr;   /**< Define for card ptr */
    bot::property<class bank> bank;    /**< Bank property to hold coins */
    bot::property<class deck> cards;   /**< Cards property to hold a deck */
    bot::property<std::vector<card_ptr>>
        table; /**< Cards on a table container property */

    /** Constructor.
     * @param users users of a room to add to the game as players.
     * @param blind_bet size of a blind bet.
     * */
    game_poker(const std::vector<bot::user_ptr>& users,
               const std::size_t& blind_bet);

    /** Function to add player.
     * Adds user as a poker player if game is not in process and
     * player is not in the game.
     * @param user pointer to a user.
     * @returns bool that is true if user was added.
     * */
    auto add_player(const bot::user_ptr user) -> bool override;

    /** Function to handle exited player.
     * Throws exception if player is not a poker player or if
     * he's not present in a game.
     * WARNING:don't handles coins yet, TODO:fix it
     * @param pl pointer to exited player.
     * */
    void handle_exit(const game::player_ptr pl) override;

    /** Game initiator.
     * Refills deck, shuffles it, fills players hands and sends game states to them.
     * */
    void init_game();

    /** Player bet handler.
     * Takes coins from a player if it's his turn, checks its size end advance game state.
     * @param user pointer to a user that made a bet.
     * @param size size of a bet.
     * */
    void handle_bet(bot::user_ptr user, const std::size_t& size);
    void handle_fold(bot::user_ptr user);

private:
    std::size_t p_last_bet;      /**< last bet to keep track */
    player_ptr p_cur_player;     /**< current player pointer */
    player_ptr p_big_blind_pl;   /**< big blinded player */
    player_ptr p_small_blind_pl; /**< small blinded player */
    std::size_t p_big_blind_bet; /**< amount of required big blind */

    auto p_player_to_it(game_poker::player_ptr p) -> players_cont::iterator;
    auto p_it_to_player(players_cont::iterator it) -> game_poker::player_ptr;
    auto p_user_to_player(const bot::user_ptr u) -> game_poker::player_ptr;
    void p_advance_place();
    void p_fill_hand(game::player_ptr pl);
    auto p_render_game_state() const -> std::string;
    auto p_render_card(const card_ptr& c) const -> std::string;
    auto p_render_coins(const bank::coins_t& c) const -> std::string;
    void p_send_state(const std::string& game_state, game::player_ptr pl);
    void p_fill_table();
};

game_poker::game_poker(const std::vector<bot::user_ptr>& users,
                       const std::size_t& blind_bet):
    games::game() {
    this->state()   = state::ended;
    p_last_bet      = 0;
    p_big_blind_bet = blind_bet;
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
    auto game_pl = players().emplace_back(new player_poker(user));
    pl           = bot::utils::dyn_cast<player_poker>(game_pl);
    bank::coins_t temp;
    for(size_t i = 0; i < 100; i++) {
        temp.emplace_back(new poker::coin(1));
    }
    pl->bank().add_coins(temp);
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
    players().erase(players().begin() + i);
    lgr << "poker: " << bot::utils::get_desc_log(pl->user())
        << " joined poker game\n";
    //TODO: handle cast->coins;
}

void game_poker::init_game() {
    cards().refill();
    cards().shuffle();
    p_fill_table();
    const auto state = p_render_game_state();
    if(players().size() >= 1) {
        p_big_blind_pl = bot::utils::dyn_cast<player_poker>(players().at(0));
        p_cur_player = p_big_blind_pl;
    }
    if(players().size() >= 2) {
        p_small_blind_pl = bot::utils::dyn_cast<player_poker>(players().at(1));
        p_cur_player = p_small_blind_pl;
    }
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
    if(pl != p_cur_player) {
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
    coins.erase(coins.begin(), coins.begin() + size);
    p_advance_place();
    auto mes = pl->user()->desc() + " made a bet:" + std::to_string(size);
    for(auto& pl: players()) {
        pl->send(mes);
    }
}

auto game_poker::p_player_to_it(game_poker::player_ptr p)
    -> players_cont::iterator {
    using namespace bot::utils;
    auto lbd = [&p](auto plr) { return dyn_cast<player_poker>(plr) == p; };
    auto it  = std::find_if(players().begin(), players().end(), lbd);
    return it;
}
auto game_poker::p_it_to_player(players_cont::iterator it)
    -> game_poker::player_ptr {
    using namespace bot::utils;
    auto pl       = *it;
    auto poker_pl = dyn_cast<player_poker>(pl);
    if(!poker_pl) {
        throw std::runtime_error(
            "player iterator doesn't hold poker player ptr");
    }
    return poker_pl;
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
    using namespace bot::utils;
    auto it = p_player_to_it(p_cur_player);
    if(*it == *players().rbegin()) {
        it = players().begin();
    }else{
        it++;
    }
    p_cur_player = dyn_cast<player_poker>(*it);
    p_cur_player->send("It's your turn.");
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
    auto mes = "Bank: " + p_render_coins(bank().coins());
    mes += "\nTable: ";
    for(auto& card: table()) {
        mes += p_render_card(card) + " ";
    }
    return mes;
}
auto game_poker::p_render_card(const card_ptr& c) const -> std::string {
    auto mes = std::to_string(c->value) + " " + c->kind.name;
    return mes;
}
auto game_poker::p_render_coins(const bank::coins_t& c) const -> std::string {
    auto mes = std::to_string(c.size());
    return mes;
}
void game_poker::p_send_state(const std::string& game_state,
                              game::player_ptr pl) {
    auto mes      = p_render_game_state();
    auto cast     = std::dynamic_pointer_cast<player_poker>(pl);
    auto& pl_bank = cast->bank();
    if(cast == p_big_blind_pl) {
        mes += "\nBig blind bet is taken from you.";
        auto big_blind_bet = pl_bank.get_coins(this->p_big_blind_bet);
        this->bank().add_coins(big_blind_bet);
        if(pl_bank.coins().size() == 0) {
            mes += "\nYour bank is empty, game over.";
            pl->send(mes);
            return;
        }
    }
    mes += "\nYour bank:";
    mes += p_render_coins(pl_bank.coins());
    if(cast != p_small_blind_pl) {
        mes += "\nHand:" + p_render_card(cast->cards().at(0)) + " " +
               p_render_card(cast->cards().at(1));
    } else {
        mes = "\nTo open your cards, bet " +
              std::to_string(p_big_blind_bet / 2) + " coins or fold.";
    }
    pl->send(mes);
}

void game_poker::p_fill_table() {
    if(table().size() == 5) {
        return;
    }
    if(table().size() > 5) {
        throw std::runtime_error("fill_table called with full table");
    }

    if(table().size() == 0) {
        table().emplace_back(cards().get_card());
        table().emplace_back(cards().get_card());
        table().emplace_back(cards().get_card());
    } else {
        table().emplace_back(cards().get_card());
    }
}

}; // namespace poker