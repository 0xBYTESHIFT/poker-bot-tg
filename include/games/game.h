#pragma once
#include "core/lazy_utils.h"
#include "core/property.h"
#include "games/player.h"

#include <vector>

namespace games {

/**
 * Class to be inherited for games classes.
 * */
class game {
public:
    using player_ptr =
        std::shared_ptr<player>; /**< Define for player pointer type. */
    using players_cont =
        std::vector<player_ptr>; /**< Define for player's container. */

    bot::property<players_cont> players; /**< Property storing players. */
    enum class state { playing, ended };
    bot::property<enum state> state;

    /**
     * Default constructor.
     * */
    game();
    /**
     * Virtual destructor for polymorphism purposes.
     * */
    virtual ~game();
    /**
     * Function to determine whether or not given user is in the game.
     * @param user user to check.
     * @returns true if user is in the game, false otherwise.
     * */
    virtual auto is_playing(const bot::user_ptr user) const -> bool;
    /**
     * Function to add someone to the game. \n
     * Derived classes should consider throwing a runtime exception
     * if user is already in the game, and also check game state. \n
     * It's virtual because derived game must add it's own player type.
     * @param user user to add.
     * @returns true if user was added, false otherwise.
     * */
    virtual auto add_player(const bot::user_ptr user) -> bool = 0;
    /**
     * Function that is called by room's class when player leaves room. \n
     * Game should process this event correctly, then player will be deleted 
     * from the game automatically, no need to call del_player, although it's not prohibited.
     * @param pl player's pointer
     * */
    virtual void handle_exit(const player_ptr pl) = 0;
    /**
     * Function to remove someone from the game.
     * If user is not in the game, throws runtime exception.
     * Derived classes should consider to do so too.
     * @param pl player to remove.
     * */
    virtual void del_player(const player_ptr& pl);
};

game::game() {}

game::~game() {};

auto game::is_playing(const bot::user_ptr user) const -> bool {
    return bot::utils::contains_if(
        players(), [&user](auto pl) { return pl->user() == user; });
}

void game::del_player(const player_ptr& pl) {
    if(!bot::utils::erase(players(), pl)) {
        throw std::runtime_error("player is not in the game");
    }
}

}; // namespace games