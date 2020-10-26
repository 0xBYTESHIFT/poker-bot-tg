#pragma once
#include "games/room.h"
#include "poker/bank.h"
#include "poker/game.h"
namespace poker{

class game_poker_room:public games::game_room{
public:
    game_poker_room(id_t id);

    void start_game();
};

game_poker_room::game_poker_room(id_t id)
    :games::game_room(id)
{
}

void game_poker_room::start_game(){
    auto &user_ptrs_vec = this->users();
    poker::bank::init_vals_t bank_config = {{1, 50},
        {5, 50},
        {10, 50},
        {25, 50},
        {50, 25},
        {100, 25}};
    this->game() = std::make_unique<poker::game_poker>(user_ptrs_vec, bank_config);
}

}