#pragma once
#include <string>
#include <vector>
#include "core/bot.h"
#include "poker/room.h"
#include "poker/game.h"

namespace poker{

class poker_bot:public bot::room_bot{
    void p_on_poker_start(bot::mes_ptr mes);
public:
    poker_bot(const std::string &token);
};

void poker_bot::p_on_poker_start(bot::mes_ptr mes){}

poker_bot::poker_bot(const std::string &token)
    :bot::room_bot(token)
{
    using args_t = std::vector<std::string>;
    const auto no_args = args_t{};

    this->room_bot::m_commands.emplace_back("poker_start",  "start poker game", no_args,
        [this](auto mes){ p_on_poker_start(mes);});

    auto &ev = m_bot.getEvents();
    for(auto &cmd:m_commands){
        ev.onCommand(cmd.cmd_word(), cmd.callback());
    }
}

void p_on_poker_start(bot::mes_ptr mes){}

};