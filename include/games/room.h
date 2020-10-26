#pragma once
#include "core/room.h"
#include "core/property.h"
#include "games/game.h"

namespace games{

class game_room:public bot::room{
public:
    using game_ptr = std::unique_ptr<games::game>;

    bot::property<game_ptr> game;

    game_room(id_t id);
};

game_room::game_room(id_t id)
    :bot::room(id)
{}

};