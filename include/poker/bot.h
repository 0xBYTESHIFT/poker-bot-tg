#pragma once
#include "core/bot.h"
#include "games/room.h"
#include "poker/game.h"
#include "poker/room.h"
#include "poker/server.h"

#include <string>
#include <vector>

namespace poker {

class poker_bot: public bot::room_bot {
    void p_on_room_poker_start(bot::mes_ptr mes);
    void p_on_room_poker_bet(bot::mes_ptr mes);
    void p_process_mes_queues(games::game_room &room);

public:
    poker_bot(const std::string& token);
};

poker_bot::poker_bot(const std::string& token): bot::room_bot(token) {
    using args_t       = std::vector<std::string>;
    const auto no_args = args_t {};

    this->s = std::make_unique<
        poker::poker_server>(); //reassign bot::server to poker::poker_server

    this->room_bot::m_commands.emplace_back(
        "poker_start", "start poker game", no_args,
        [this](auto mes) { p_on_room_poker_start(mes); });

    this->room_bot::m_commands.emplace_back(
        "poker_bet", "make a bet in poker", args_t {"amount"},
        [this](auto mes) { p_on_room_poker_bet(mes); });

    auto& ev = m_bot.getEvents();
    for(auto& cmd: m_commands) {
        ev.onCommand(cmd.cmd_word(), cmd.callback());
    }
}

void poker_bot::p_process_mes_queues(games::game_room &room){
    for(auto& pl: room.game()->players()) {
        auto& mes_q = pl->mes_queue();
        while(!mes_q.empty()) {
            auto& mes = mes_q.front();
            api.sendMessage(pl->user()->id(), mes);
            mes_q.pop();
        }
    }
}

void poker_bot::p_on_room_poker_start(bot::mes_ptr mes) {
    ROOM_BOT_PREPARE(mes);
    using namespace bot::utils;

    auto room = dyn_cast<poker::game_poker_room>(user->current_room());
    room->start_game();
    p_process_mes_queues(*room);
}

void poker_bot::p_on_room_poker_bet(bot::mes_ptr mes) {
    ROOM_BOT_PREPARE(mes);
    using namespace bot::utils;

    auto room = dyn_cast<poker::game_poker_room>(user->current_room());
    if(!room->game()){
        return;
    }
    auto poker = dyn_cast<poker::game_poker>(room->game());
    auto words = StringTools::split(mes->text, ' ');
    auto size = std::stoi(words.at(1));
    poker->handle_bet(user, size);
    p_process_mes_queues(*room);
}

}; // namespace poker