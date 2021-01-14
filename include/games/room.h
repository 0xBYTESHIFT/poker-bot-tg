#pragma once
#include "core/property.h"
#include "core/room.h"
#include "games/game.h"

namespace games {

/**
 * Game room's class.
 * */
class game_room: public bot::room {
public:
    using game_ptr = std::unique_ptr<games::game>; /**< Define for game pointer. */

    bot::property<game_ptr> game; /**< Property storing game. */

    /**
     * Construstor.
     * @param id id to identify room in a containers.
     * */
    game_room(id_t id);

    /**
     * Function to try to delete player from the game. \n
     * It checks if game ptr is existing and if it's so, calls game::handle_exit
     * and game::del_player. \n
     * May be overloaded in derived classes.
     * @param user ptr to a user that exited the room.
     * */
    virtual void del_user(bot::user_ptr user) override;
};

game_room::game_room(id_t id): bot::room(id) { }

void game_room::del_user(bot::user_ptr user) {
    if(!game()) {
        return; //no need to delete player from game
    }
    auto player_it = bot::utils::find_if(game()->players(), [&](auto pl) { return pl->user() == user; });
    if(player_it != game()->players().end()) {
        game()->handle_exit(*player_it);
        try {
            game()->del_player(*player_it);
        } catch(...) { //in case if "handle_exit" deletes it
        }
    }
}

}; // namespace games