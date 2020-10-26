#pragma once
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <set>
#include <random>
#include "core/property.h"
#include "core/room.h"
#include "core/user.h"
#include "core/datatypes.h"
#include "core/utils.h"

namespace bot{
class token_generator{
    static inline std::set<std::string> p_tokens;
    const static size_t p_token_len = 8; 
    const static inline std::string p_alphabet =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    static inline std::random_device rd;
    static inline std::mt19937 p_gen = std::mt19937(rd());
    static inline std::uniform_int_distribution<unsigned> dist =
        std::uniform_int_distribution<unsigned>(0, 51); //p_alphabet.size()-1
public:
    static std::string gen();
};

std::string token_generator::gen(){
    std::string result(p_token_len, ' ');
    bool end = false;
    do{
        std::generate(result.begin(), result.end(),
            [&](){ return p_alphabet.at(dist(p_gen)); });
        if(p_tokens.find(result) == p_tokens.end()){
            end = true;
            p_tokens.emplace(result);
        }
    }while(!end);
    return result;
};

class server{
    static inline id_t p_last_room_id = 0;
    logger &lgr;
public:
    using room_cont = std::vector<room_ptr>;
    using user_cont = std::map<identifyable::id_t, user_ptr>;

    property<room_cont> rooms = {};
    property<room_ptr> lobby = nullptr;
    property<user_cont> users = {};

    server();
    virtual user_ptr get_user(id_t id)const;
    virtual room_ptr get_room(const room::token_t &token)const;
    virtual room_ptr create_room(user_ptr user);
    virtual void on_user_connect(user_ptr user);
    virtual void on_user_disconnect(user_ptr user);
    virtual void on_room_empty(room_ptr room);
};

server::server()
    :lgr(logger::get_logger())
{
    lobby = std::make_shared<room>(0);
    lobby()->name = std::string("lobby");
    lobby()->token() = token_generator::gen();
}

user_ptr server::get_user(id_t id)const{
    auto user_it = users().find(id);
    if(user_it != users().end()){
        return user_it->second;
    }
    return nullptr;
}

room_ptr server::get_room(const room::token_t &token)const{
    auto room_it = std::find_if(rooms().begin(), rooms().end(),
        [&token](auto room){ return room->token() == token; });
    if(room_it != rooms().end()){
        return *room_it;
    }
    return nullptr;
}

room_ptr server::create_room(user_ptr user){
    lobby()->del_user(user);

    auto room = std::make_shared<class room>(++p_last_room_id);
    room->token() = token_generator::gen();
    room->add_user(user);
    room->owner() = user;
    user->room() = room;
    rooms().emplace_back(room);

    lgr << "server: user"<<bot::get_desc_log(user)<<" created room"<<bot::get_desc(room)<<"\n";
    return room;
}

void server::on_user_connect(user_ptr user){
    users().emplace(user->id, user);
    user->token = token_generator::gen();
}

void server::on_user_disconnect(user_ptr user){
    users().erase(user->id);
}

void server::on_room_empty(room_ptr room){
    if(!room->users().empty()){
        lgr << "E server: called on_empty handler on non-empty room"<<bot::get_desc(room)<<"\n";
        return;
    }
    auto &rooms = this->rooms();
    auto room_it = std::find(rooms.begin(), rooms.end(), room);
    if(room_it == rooms.end()){ return; }
    rooms.erase(room_it);
    lgr << "server: room"<<bot::get_desc(room)<<" removed\n";
}

};