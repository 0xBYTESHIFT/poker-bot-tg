#pragma once
#include "core/bot.h"
#include "poker/game.h"
#include "poker/room.h"
#include "poker/server.h"

#include <string>
#include <vector>

namespace poker {

class poker_bot: public bot::room_bot {
    void p_on_room_poker_start(bot::mes_ptr mes);

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

    auto& ev = m_bot.getEvents();
    for(auto& cmd: m_commands) {
        ev.onCommand(cmd.cmd_word(), cmd.callback());
    }
}

void poker_bot::p_on_room_poker_start(bot::mes_ptr mes) {
    auto id   = mes->chat->id;
    auto& s   = *this->s.get();
    auto tpl  = p_process_cmd(mes);
    auto user = std::get<0>(tpl);
    auto cmd  = std::get<1>(tpl);
    if(!user || !cmd) {
        return;
    }

    auto room = std::dynamic_pointer_cast<poker::game_poker_room>(user->room());
    room->start_game();
}

}; // namespace poker