#pragma once
#include "components/logger.hpp"
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
    using player_ptr = std::shared_ptr<player_poker>; /**< Define for poker player ptr */
    bot::property<class bank> bank;                   /**< Bank property to hold coins */
    bot::property<class deck> cards;                  /**< Cards property to hold a deck */
    bot::property<std::vector<card>> table;           /**< Cards on a table container property */

    /** Constructor.
     * @param users users of a room to add to the game as players.
     * @param blind_bet size of a blind bet.
     * */
    game_poker(const std::vector<bot::user_ptr>& users, std::size_t blind_bet);

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
    void handle_bet(bot::user_ptr user, std::size_t size);
    void handle_fold(bot::user_ptr user);

private:
    std::size_t p_last_bet;                          /**< last bet to keep track */
    player_ptr p_cur_player;                         /**< current player pointer */
    player_ptr p_big_blind_pl;                       /**< big blinded player */
    player_ptr p_small_blind_pl;                     /**< small blinded player */
    std::size_t p_big_blind_bet;                     /**< amount of required big blind */
    std::map<game_poker::player_ptr, size_t> p_bets; /**< container which represents players bets summ*/
    bool p_small_blind_made_bet;                     /**< flag to indicate small bet status */

    auto p_player_to_it(game_poker::player_ptr p) -> players_cont::iterator;
    auto p_it_to_player(players_cont::iterator it) -> game_poker::player_ptr;
    auto p_user_to_player(const bot::user_ptr u) -> game_poker::player_ptr;
    void p_advance_place();
    void p_fill_hand(game::player_ptr pl);
    void p_handle_bet(game::player_ptr pl, size_t);
    auto p_render_game_state() const -> std::string;
    auto p_render_card(const card& c) const -> std::string;
    auto p_render_coins(const bank::coins_t& c) const -> std::string;
    void p_send_state(const std::string& game_state, game::player_ptr pl);
    void p_fill_table();
};

game_poker::game_poker(const std::vector<bot::user_ptr>& users, std::size_t blind_bet): games::game() {
    this->state()          = state::ended;
    p_big_blind_bet        = blind_bet;
    p_small_blind_made_bet = false;
    for(auto& u: users) {
        add_player(u);
    }
}

auto game_poker::add_player(const bot::user_ptr user) -> bool {
    auto lgr    = get_logger();
    auto prefix = fmt::format("game_poker::add_player {}", user->log_desc());
    auto pl     = p_user_to_player(user);

    if(state() == state::playing) {
        m_lgr.debug("{} can't join during the game", prefix);
        return false;
    }

    if(pl) {
        m_lgr.debug("{} can't join, already in the game", prefix);
        return false;
    }

    auto game_pl = players().emplace_back(new player_poker(user));
    pl           = bot::utils::dyn_cast<player_poker>(game_pl);
    bank::coins_t temp;
    for(size_t i = 0; i < 100; i++) {
        temp.emplace_back(new poker::coin(1));
    }
    pl->bank().add_coins(temp);
    m_lgr.info("{} joined poker game", user->log_desc());
    return true;
}

void game_poker::handle_exit(const game::player_ptr pl) {
    auto lgr    = get_logger();
    auto prefix = fmt::format("game_poker::handle_exit {}", pl->user()->log_desc());
    auto cast   = std::dynamic_pointer_cast<const poker::player_poker>(pl);
    if(!cast) {
        auto mes = fmt::format("{} wrong player class in the poker game", prefix);
        lgr.error(mes);
        throw std::runtime_error(mes);
    }
    auto it = bot::utils::find(players(), pl);
    if(it == players().end()) {
        auto mes = fmt::format("{} no player in container", prefix);
        lgr.error(mes);
        throw std::runtime_error(mes);
    }
    players().erase(it);
    p_bets.erase(std::const_pointer_cast<poker::player_poker>(cast)); //TODO:test
    auto mes = fmt::format("{} exited poker game", prefix);
    lgr.debug(mes);
}

void game_poker::init_game() {
    cards().refill();
    cards().shuffle();
    p_fill_table();

    if(players().size() >= 1) {
        p_big_blind_pl = bot::utils::dyn_cast<player_poker>(players().at(0));
        p_cur_player   = p_big_blind_pl;
    }
    if(players().size() >= 2) {
        p_small_blind_pl = bot::utils::dyn_cast<player_poker>(players().at(1));
    }
    for(auto& pl: players()) {
        p_fill_hand(pl);
    }
    if(p_big_blind_pl) {
        auto tmp_pl   = p_big_blind_pl;
        auto bet_size = p_big_blind_bet;
        auto& bank    = tmp_pl->bank();
        if(bank.coins().size() >= bet_size) {
            auto tmp = bank.get_coins(bet_size);
            this->bank().add_coins(tmp);
            auto mes = fmt::format("Big blind was taken from you ({}))", bet_size);
            tmp_pl->send(std::move(mes));
            p_bets[tmp_pl] += bet_size;
            p_last_bet = bet_size;
        } else {
            //TODO:handle properly
        }
    }
    if(p_small_blind_pl) {
        auto tmp_pl   = p_small_blind_pl;
        auto bet_size = p_big_blind_bet / 2;
        auto& bank    = tmp_pl->bank();
        if(bank.coins().size() >= bet_size) {
            auto tmp = bank.get_coins(bet_size);
            this->bank().add_coins(tmp);
            auto mes = fmt::format("Small blind was taken from you ({})", bet_size);
            tmp_pl->send(mes);
            p_bets[tmp_pl] += bet_size;
        } else {
            //TODO:handle properly
        }
    }
    const auto state = p_render_game_state();
    for(auto& pl: players()) {
        p_send_state(state, pl);
    }
    p_advance_place(); //advance from big blind to small blind
}
void game_poker::handle_bet(bot::user_ptr user, std::size_t size) {
    auto lgr    = get_logger();
    auto prefix = fmt::format("game_poker::handle_bet {}", user->log_desc());
    auto pl     = p_user_to_player(user);
    if(!pl) {
        lgr.error("{} no such player", prefix);
        return;
    }
    if(pl != p_cur_player) {
        lgr.debug("{} bet in wrong order", prefix);
        pl->send("It's not your turn to make a bet");
        return;
    }
    p_handle_bet(pl, size);
}

auto game_poker::p_player_to_it(game_poker::player_ptr p) -> players_cont::iterator {
    using namespace bot::utils;
    auto lbd = [&p](auto plr) { return dyn_cast<player_poker>(plr) == p; };
    auto it  = std::find_if(players().begin(), players().end(), lbd);
    return it;
}
auto game_poker::p_it_to_player(players_cont::iterator it) -> game_poker::player_ptr {
    using namespace bot::utils;
    auto lgr      = get_logger();
    auto prefix   = "game_poker::p_it_to_player";
    auto pl       = *it;
    auto poker_pl = dyn_cast<player_poker>(pl);
    if(!poker_pl) {
        auto mes = fmt::format("{} player iterator doesn't hold poker player ptr", prefix);
        lgr.error(mes);
        throw std::runtime_error(mes);
    }
    return poker_pl;
}
auto game_poker::p_user_to_player(const bot::user_ptr u) -> game_poker::player_ptr {
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
    auto lgr    = get_logger();
    auto prefix = "game_poker::p_advance_place";
    auto it     = p_player_to_it(p_cur_player);
    if(it == players().end()) {
        auto mes = fmt::format("{} player iterator doesn't hold poker player ptr", prefix);
        lgr.error(mes);
        throw std::runtime_error(mes);
    }
    if(*it == *players().rbegin()) {
        it = players().begin();
    } else {
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
void game_poker::p_handle_bet(game::player_ptr pl, size_t size) {
    auto lgr        = get_logger();
    auto prefix     = fmt::format("game_poker::p_handle_bet {}", pl->user()->log_desc());
    auto cast       = std::dynamic_pointer_cast<player_poker>(pl);
    auto& coins     = cast->bank().coins();
    auto& game_bank = bank().coins();
    if(coins.size() < size) {
        auto mes = fmt::format("{} attempt to bet {}, but bank is:{}", prefix, size, coins.size());
        lgr.debug(mes);
        auto mes_pl = fmt::format("You can't make that bet, your bank is:{}", coins.size());
        pl->send(mes_pl);
        return;
    }
    if(cast == p_small_blind_pl) {
        if(!p_small_blind_made_bet && size != p_big_blind_bet / 2) {
            auto mes = fmt::format("Your bet can't be other than {}", p_big_blind_bet / 2);
            pl->send(mes);
            return;
        } else {
            p_small_blind_made_bet = true;
        }
    } else {
        if(p_bets[cast] + size < p_last_bet) {
            auto mes = fmt::format("Your bet can't be lower than {}", p_last_bet - p_bets[cast]);
            pl->send(mes);
            return;
        }
    }
    m_lgr.debug("{} made a bet:{}", prefix, size);
    if(p_bets[cast] > p_last_bet) {
        p_last_bet = p_bets[cast];
    }
    std::move(coins.begin(), coins.begin() + size, std::back_inserter(game_bank));
    coins.erase(coins.begin(), coins.begin() + size);
    p_advance_place();
    /*
    auto mes = pl->user()->desc() + " made a bet:" + std::to_string(size);
    for(auto& pl: players()) {
        pl->send(mes);
    }
    */

    p_bets[cast] += size;
    {
        size_t prev_bet = p_bets.begin()->second;
        bool equals =
            std::all_of(p_bets.begin(), p_bets.end(), [&prev_bet](auto pair) { return pair.second == prev_bet; });
        if(equals) {
            if(table().size() != 5) {
                p_fill_table();
                for(auto& el: p_bets) {
                    el.second = 0;
                }
            } else {
                //TODO:handle properly
            }
        }
    }
    auto state = p_render_game_state();
    for(auto& pl: players()) {
        p_send_state(state, pl);
    }
}
auto game_poker::p_render_game_state() const -> std::string {
    auto mes = "Bank: " + p_render_coins(bank().coins());
    mes += "\nTable: ";
    for(auto& card: table()) {
        mes += p_render_card(card) + " ";
    }
    for(auto [pl_ptr, count]: p_bets) {
        mes += "\n" + pl_ptr->user()->desc() + " bet:" + std::to_string(count);
    }
    return mes;
}
auto game_poker::p_render_card(const card& c) const -> std::string {
    const static std::map<std::string, std::string> kind_to_emoji {
        {"pikes", "\xE2\x99\xA0"}, {"hearts", "\xE2\x99\xA5"}, {"tiles", "\xE2\x99\xA6"}, {"clovers", "\xE2\x99\xA3"}};
    const static std::map<int, std::string> value_to_letter {{11, "J"}, {12, "D"}, {13, "K"}, {14, "A"}};
    std::string mes;
    if(c.value < 11) {
        mes += std::to_string(c.value);
    } else {
        mes += value_to_letter.at(c.value);
    }
    mes += " " + kind_to_emoji.at(c.kind.name);
    return mes;
}
auto game_poker::p_render_coins(const bank::coins_t& c) const -> std::string {
    auto mes = std::to_string(c.size());
    return mes;
}
void game_poker::p_send_state(const std::string& game_state, game::player_ptr pl) {
    auto mes      = game_state;
    auto cast     = std::dynamic_pointer_cast<player_poker>(pl);
    auto& pl_bank = cast->bank();
    mes += "\nYour bank:" + p_render_coins(pl_bank.coins());
    if(cast != p_small_blind_pl || p_small_blind_made_bet) {
        auto c1 = p_render_card(cast->cards().at(0));
        auto c2 = p_render_card(cast->cards().at(1));
        mes += fmt::format("\nHand: {} {}", c1, c2);
    } else {
        mes += fmt::format("\nTo open your cards, bet {} coins", p_big_blind_bet / 2);
    }
    pl->send(mes);
}

void game_poker::p_fill_table() {
    auto lgr    = get_logger();
    auto prefix = "game_poker::p_fill_table";

    if(table().size() == 5) {
        return;
    }
    if(table().size() > 5) {
        auto mes = fmt::format("{} fill_table called with full table", prefix);
        lgr.error(mes);
        throw std::runtime_error(mes);
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