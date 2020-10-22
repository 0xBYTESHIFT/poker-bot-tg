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
    const TgBot::Api& api;

    void p_on_start(mes_ptr mes);
    void p_on_stop(mes_ptr mes);
    void p_on_any(mes_ptr mes);

    void p_on_room_create_request(mes_ptr mes);
    void p_on_room_close_request(mes_ptr mes);
    void p_on_room_join_request(mes_ptr mes);
    void p_on_room_list_request(mes_ptr mes);
    void p_on_room_mute_request(mes_ptr mes);
    void p_on_room_unmute_request(mes_ptr mes);
    void p_on_room_ban_request(mes_ptr mes);
    void p_on_room_unban_request(mes_ptr mes);
    void p_on_room_kick_request(mes_ptr mes);
public:
    poker_bot(const std::string &token);

    void start();
};



void poker_bot::p_on_start(mes_ptr mes){
    auto id = mes->chat->id;
    api.sendMessage(id, "Hi!");
    auto user = std::make_shared<class user>(id);
    user->name() = mes->chat->firstName;
    if(!mes->chat->lastName.empty()){
        user->name() += " "+mes->chat->lastName;
    }

    s.lobby()->add_user(user);
    user->room() = s.lobby();
    s.on_user_connect(user);
}

void poker_bot::p_on_stop(mes_ptr mes){
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
    auto id = mes->chat->id;

    if (StringTools::startsWith(mes->text, "/start")) { return; }
    if (StringTools::startsWith(mes->text, "/stop")) { return; }
    if (StringTools::startsWith(mes->text, "/create")) { return; }
    if (StringTools::startsWith(mes->text, "/close")) { return; }
    if (StringTools::startsWith(mes->text, "/mute")) { return; }
    if (StringTools::startsWith(mes->text, "/unmute")) { return; }
    if (StringTools::startsWith(mes->text, "/kick")) { return; }
    if (StringTools::startsWith(mes->text, "/ban")) { return; }
    if (StringTools::startsWith(mes->text, "/unban")) { return; }
    if (StringTools::startsWith(mes->text, "/list")) { return; }
    auto user = s.get_user(id);
    if(!user){
        return;
    }

    auto &room = user->room();
    room->process_mes(user, mes);
    if(room->muted().find(user) != room->muted().end()){ return; }

    std::string relay_mes = user->name()+":"+mes->text;
    auto &users = user->room()->users();
    for(const auto &u:users){
        if(u == user){ continue; }
        api.sendMessage(u->id, relay_mes);
    }
}

void poker_bot::p_on_room_create_request(mes_ptr mes){
    auto id = mes->chat->id;
    auto user = s.get_user(id);
    if(!user){ return; }

    auto room = s.create_room(user); //places user in that room too

    std::string response = "Welcome to new room,\n"
        "Send this token to your friends so they could join you:";
    api.sendMessage(id, response);
    response = room->token();
    api.sendMessage(id, response);
}

void poker_bot::p_on_room_close_request(mes_ptr mes){
    auto id = mes->chat->id;
    auto user = s.get_user(id);
    if(!user){ return; }

    auto room = user->room();
    if(!room || room == s.lobby()){ return; } //don't delete null room or lobby
    room->del_user(user);
    if(room->users().empty()){
        s.on_room_empty(room);
    }

    s.lobby()->add_user(user);
    user->room() = s.lobby();
    api.sendMessage(id, "Welcome to lobby!");
}
void poker_bot::p_on_room_join_request(mes_ptr mes){
    auto id = mes->chat->id;
    auto user = s.get_user(id);
    if(!user){ return; }

    auto words = StringTools::split(mes->text, ' ');
    std::string response;
    if(words.size() != 2){
        response = "Usage: /join [room_token]";
    }else{
        const auto &token = words.at(1);
        auto room = s.get_room(token);
        if(!room){
            response = "No room with token "+token;
        }else if(room->banned().find(user) != room->banned().end()){
            response = "You are banned from joining room "+room->desc();
        }else{
            response = "Welcome to room "+room->desc();
            user->room()->del_user(user); //delete from previous room
            room->add_user(user); //place in joined room
            user->room() = room; //save joined room in user too

            std::string broadcast_mes = "User "+user->desc()+" joined";
            for(auto &u:room->users()){
                if(u == user){ continue; }
                api.sendMessage(id, broadcast_mes);
            }
        }
    }
    api.sendMessage(id, response);
}
void poker_bot::p_on_room_list_request(mes_ptr mes){
    auto id = mes->chat->id;
    auto user = s.get_user(id);
    if(!user){ return; }

    auto &room = user->room();
    auto &users = room->users();
    std::string response = "token name [muted]\n";
    for(auto &u:users){
        response += u->token()+" ";
        response += u->name()+" "; //"unnamed" if empty
        if(room->muted().find(u) != room->muted().end()){
            response += "muted";
        }
        response += "\n";
    }
    api.sendMessage(id, response);
}
void poker_bot::p_on_room_kick_request(mes_ptr mes){
    auto id = mes->chat->id;
    auto user = s.get_user(id);
    if(!user){ return; }

    auto &room = user->room();
    auto &users = room->users();
    std::string response;

    if(room->owner() != user){
        response = "You are not allowed to do this";
    }else{
        auto words = StringTools::split(mes->text, ' ');
        if(words.size() != 2){
            response = "Usage: /kick [token]";
        }else{
            auto token = words.at(1);
            auto user_kicked = room->get_user(token);
            if(!user_kicked){
                response = "No user with token "+token+" in this room";
            }else{
                room->del_user(user_kicked); //remove user from kicked room
                s.lobby()->add_user(user_kicked); //place kicked user in lobby
                user_kicked->room() = s.lobby(); //save lobby as user's new room

                api.sendMessage(user_kicked->id, "You were kicked from room "+room->desc());
                response = user_kicked->desc()+" was kicked from this room";
                for(auto &u:users){
                    if(u == user){ continue; }
                    api.sendMessage(u->id, response);
                }
            }
        }
    }
    api.sendMessage(id, response);
}
void poker_bot::p_on_room_mute_request(mes_ptr mes){
    auto id = mes->chat->id;
    auto user = s.get_user(id);
    if(!user){ return; }
    auto &room = user->room();
    auto &users = room->users();
    std::string response;

    if(room->owner() != user){
        response = "You are not allowed to do this";
    }else{
        auto words = StringTools::split(mes->text, ' ');
        if(words.size() != 2){
            response = "Usage: /mute [token]";
        }else{
            auto token = words.at(1);
            auto user_muted = room->get_user(token);
            if(!user_muted){
                response = "No user with token "+token+" in this room";
            }else{
                room->muted().emplace(user_muted);

                api.sendMessage(user_muted->id, "You were muted in room "+room->desc());
                response = user_muted->desc()+" was muted in this room";
                for(auto &u:users){
                    if(u == user){ continue; }
                    api.sendMessage(u->id, response);
                }
            }
        }
    }
    api.sendMessage(id, response);
}
void poker_bot::p_on_room_unmute_request(mes_ptr mes){
    auto id = mes->chat->id;
    auto user = s.get_user(id);
    if(!user){ return; }
    auto &room = user->room();
    auto &users = room->users();
    std::string response;

    if(room->owner() != user){
        response = "You are not allowed to do this";
    }else{
        auto words = StringTools::split(mes->text, ' ');
        if(words.size() != 2){
            response = "Usage: /unmute [token]";
        }else{
            auto token = words.at(1);
            auto user_unmuted_it = std::find_if(room->muted().begin(), room->muted().end(),
                [&token](auto u){ return u->token() == token; });
            if(user_unmuted_it == room->muted().end()){
                response = "No user with token "+token+" was muted in this room";
            }else{
                auto user_unmuted = *user_unmuted_it;
                room->muted().erase(user_unmuted);

                api.sendMessage(user_unmuted->id, "You were unmuted in room "+room->desc());
                response = user_unmuted->desc()+" was unmuted in this room";
                for(auto &u:users){
                    if(u == user){ continue; }
                    api.sendMessage(u->id, response);
                }
            }
        }
    }
    api.sendMessage(id, response);
}
void poker_bot::p_on_room_ban_request(mes_ptr mes){
    auto id = mes->chat->id;
    auto user = s.get_user(id);
    if(!user){ return; }

    auto &room = user->room();
    auto &users = room->users();
    std::string response;

    if(room->owner() != user){
        response = "You are not allowed to do this";
    }else{
        auto words = StringTools::split(mes->text, ' ');
        if(words.size() != 2){
            response = "Usage: /ban [token]";
        }else{
            auto token = words.at(1);
            auto user_banned = room->get_user(token);
            if(!user_banned){
                response = "No user with token "+token+" in this room";
            }else{
                room->banned().emplace(user_banned);
                room->del_user(user_banned); //remove user from room
                s.lobby()->add_user(user_banned); //place user in lobby
                user_banned->room() = s.lobby(); //save lobby as user's new room

                api.sendMessage(user_banned->id, "You were banned in room "+room->desc());
                response = user_banned->desc()+" was banned from this room";
                for(auto &u:users){
                    if(u == user){ continue; }
                    api.sendMessage(u->id, response);
                }
            }
        }
    }
    api.sendMessage(id, response);
}
void poker_bot::p_on_room_unban_request(mes_ptr mes){
    auto id = mes->chat->id;
    auto user = s.get_user(id);
    if(!user){ return; }

    auto &room = user->room();
    auto &users = room->users();
    std::string response;

    if(room->owner() != user){
        response = "You are not allowed to do this";
    }else{
        auto words = StringTools::split(mes->text, ' ');
        if(words.size() != 2){
            response = "Usage: /unban [token]";
        }else{
            auto token = words.at(1);
            auto user_unbanned_it = std::find_if(room->banned().begin(), room->banned().end(),
                [&token](auto u){ return u->token() == token; });
            if(user_unbanned_it == room->banned().end()){
                response = "No user with token "+token+" was banned in this room";
            }else{
                auto user_unbanned = *user_unbanned_it;
                room->banned().erase(user_unbanned);

                api.sendMessage(user_unbanned->id, "You were unbanned in room "+room->desc());
                response = user_unbanned->desc()+" was unbanned in this room";
                for(auto &u:users){
                    if(u == user){ continue; }
                    api.sendMessage(u->id, response);
                }
            }
        }
    }
    api.sendMessage(id, response);
}

poker_bot::poker_bot(const std::string &token)
    :api(m_bot.getApi()),
    m_bot(token)
{
    auto &ev = m_bot.getEvents();
    ev.onCommand("start",   [this](auto mes){ p_on_start(mes); });
    ev.onCommand("stop",    [this](auto mes){ p_on_stop(mes); });
    ev.onCommand("create",  [this](auto mes){ p_on_room_create_request(mes); });
    ev.onCommand("close",   [this](auto mes){ p_on_room_close_request(mes); });
    ev.onCommand("join",    [this](auto mes){ p_on_room_join_request(mes); });
    ev.onCommand("list",    [this](auto mes){ p_on_room_list_request(mes); });
    ev.onCommand("kick",    [this](auto mes){ p_on_room_kick_request(mes); });
    ev.onCommand("mute",    [this](auto mes){ p_on_room_mute_request(mes); });
    ev.onCommand("unmute",  [this](auto mes){ p_on_room_unmute_request(mes); });
    ev.onCommand("ban",     [this](auto mes){ p_on_room_ban_request(mes); });
    ev.onCommand("unban",   [this](auto mes){ p_on_room_unban_request(mes); });
    ev.onAnyMessage([this](mes_ptr mes) { p_on_any(mes); });
}

void poker_bot::start(){
    printf("Bot username: %s\n", m_bot.getApi().getMe()->username.c_str());
    TgBot::TgLongPoll longPoll(m_bot);
    while (true) {
        longPoll.start();
    }
}