#pragma once
#include "core/room.h"
#include "core/property.h"
#include "games/game.h"

namespace games{

/**
 * Game room's class.
 * */
class game_room:public bot::room{
public:
    using game_ptr = std::unique_ptr<games::game>; /**< Define for game pointer. */

    bot::property<game_ptr> game; /**< Property storing game. */

    /**
     * Construstor.
     * @param id id to identify room in a containers.
     * */
    game_room(id_t id);
};

game_room::game_room(id_t id)
    :bot::room(id)
{}

};