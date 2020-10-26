#pragma once
#include "games/room.h"

namespace poker{

class game_poker_room:public games::game_room{
public:
    game_poker_room(id_t id);
};

game_poker_room::game_poker_room(id_t id)
    :games::game_room(id)
{
    //TODO:init game on start
    // this->game() = std::make_unique<poker::game_poker>();
}

}