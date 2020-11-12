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

game_poker_room::game_poker_room(id_t id): games::game_room(id) {}

void game_poker_room::start_game() {
    auto& user_ptrs_vec   = this->users();
    std::size_t bank_size = 1000;
    auto min_bet = std::max((int)(bank_size*1./100*2), (int)2);
    auto poker            = new game_poker(user_ptrs_vec, bank_size, min_bet);
    this->game()          = std::unique_ptr<poker::game_poker>(poker);
    poker->init_game();
}

} // namespace poker