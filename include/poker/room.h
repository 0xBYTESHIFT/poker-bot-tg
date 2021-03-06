#pragma once
#include "games/room.h"
#include "poker/bank.h"
#include "poker/game.h"
namespace poker {

class game_poker_room: public games::game_room {
public:
    game_poker_room(id_t id);

    void start_game();
};

game_poker_room::game_poker_room(id_t id): games::game_room(id) { }

void game_poker_room::start_game() {
    auto& user_ptrs_vec = this->users();
    auto min_bet        = 10;
    auto poker          = new game_poker(user_ptrs_vec, min_bet);
    this->game()        = std::unique_ptr<poker::game_poker>(poker);
    poker->init_game();
}

} // namespace poker