#pragma once
#include <tgbot/tgbot.h>
#include <algorithm>
#include "datatypes.h"
#include "user.h"
#include "room.h"
#include "server.h"

class poker_bot{
protected:
    TgBot::Bot m_bot;
    server s;

    void p_on_start(mes_ptr mes);
    void p_on_quit(mes_ptr mes);
    void p_on_any(mes_ptr mes);
public:
    poker_bot(const std::string &token);

    void start();
};



void poker_bot::p_on_start(mes_ptr mes){
    auto &api = m_bot.getApi();
    auto id = mes->chat->id;
    api.sendMessage(id, "Hi!");
    auto user = std::make_shared<class user>(id);
    user->name() = mes->chat->firstName;
    if(!mes->chat->lastName.empty()){
        user->name() += " "+mes->chat->lastName;
    }

    s.lobby()->add_user(user);
    user->room() = s.lobby();
    s.users().emplace(id, user);
}

void poker_bot::p_on_quit(mes_ptr mes){
    auto &api = m_bot.getApi();
    auto id = mes->chat->id;
    auto user = s.get_user(id);
    if(!user){
        return;
    }
    user->room()->del_user(user);
    {
        auto& serv_users = s.users();
        serv_users.erase(id);
    }
}

void poker_bot::p_on_any(mes_ptr mes){
    auto &api = m_bot.getApi();
    auto id = mes->chat->id;

    if (StringTools::startsWith(mes->text, "/start")) { return; }
    if (StringTools::startsWith(mes->text, "/quit")) { return; }
    auto user = s.get_user(id);
    if(!user){
        return;
    }
    user->room()->process_mes(user, mes);
    api.sendMessage(id, "Your message is: " + mes->text);
}

poker_bot::poker_bot(const std::string &token)
    :m_bot(token)
{
    auto &ev = m_bot.getEvents();
    ev.onCommand("start", [this](mes_ptr mes){ p_on_start(mes); });
    ev.onCommand("quit", [this](mes_ptr mes){ p_on_quit(mes); });
    ev.onAnyMessage([this](mes_ptr mes) { p_on_any(mes); });
}

void poker_bot::start(){
    printf("Bot username: %s\n", m_bot.getApi().getMe()->username.c_str());
    TgBot::TgLongPoll longPoll(m_bot);
    while (true) {
        longPoll.start();
    }
}