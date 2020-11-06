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
/**
 * Utility class to generate random tokens.
 * */
class token_generator{
    static inline std::set<std::string> p_tokens; /**< Set to keep already generated tokens */
    const static size_t p_token_len = 8;  /**< Define for token's length */ 
    const static inline std::string p_alphabet = /**< Alphabet for generating tokens */
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
public:
    /**
     * Func that generates a random unique token.
     * @returns a token
     * */
    static std::string gen();
};

std::string token_generator::gen(){
    static std::random_device rd;
    static std::mt19937 p_gen = std::mt19937(rd());
    static std::uniform_int_distribution<unsigned> dist =
        std::uniform_int_distribution<unsigned>(0, p_alphabet.size()-1);

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

/**
 * Server class to hold users and rooms
 * */
class server{
    static inline id_t p_last_room_id = 0; /**< Last room id to keep generated room's unique */
protected:
    logger &lgr; /**< Logger reference */
    /**
     * Function to return new room's id
     * */
    id_t p_get_room_id();
public:
    using room_cont = std::vector<room_ptr>; /**< Define for rooms container */ 
    using user_cont = std::map<identifyable::id_t, user_ptr>; /**< Define for users container */

    property<room_cont> rooms = {}; /**< Rooms' pointers container */
    property<room_ptr> lobby = nullptr; /**< Pointer to the lobby room */
    property<user_cont> users = {}; /**< Users' container */

    /**
     * Default constructor, initializes the lobby
     * */
    server();
    /**
     * Function to find a user by their's id.
     * @returns user_ptr if they are found
     * */
    virtual user_ptr get_user(id_t id)const;
    /**
     * Function to find a room by it's token.
     * @returns room_ptr if it's found
     * */
    virtual room_ptr get_room(const room::token_t &token)const;
    /**
     * Function to create a room when user requested it.
     * @param user ptr to a user that requester room creation.
     * @returns pointer to a created room
     * */
    virtual room_ptr create_room(user_ptr user);
    /**
     * Function to be called when user connects to the server.
     * @param user ptr to a user that connected to the server.
     * */
    virtual void on_user_connect(user_ptr user);
    /**
     * Function to be called when user disconnects to the server.
     * @param user ptr to a user that diconnected to the server.
     * */
    virtual void on_user_disconnect(user_ptr user);
    /**
     * Function to be called when room became empty.
     * By default, this function deletes the room.
     * @param room ptr to a room that became empty
     * */
    virtual void on_room_empty(room_ptr room);
};

server::server()
    :lgr(logger::get_instance())
{
    lobby = std::make_shared<room>(0);
    lobby()->name = std::string("lobby");
    lobby()->token() = token_generator::gen();
}

id_t server::p_get_room_id(){
    return ++p_last_room_id;
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

    auto room = std::make_shared<class room>(p_get_room_id());
    room->token() = token_generator::gen();
    room->add_user(user);
    room->owner() = user;
    user->room() = room;
    rooms().emplace_back(room);

    lgr << "server: user"<<utils::get_desc_log(user)<<" created room"<<utils::get_desc(room)<<"\n";
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
        lgr << "E server: called on_empty handler on non-empty room"<<bot::utils::get_desc(room)<<"\n";
        return;
    }
    if(utils::erase(rooms(), room)){
        lgr << "server: room"<<bot::utils::get_desc(room)<<" removed\n";
    }else{
        throw std::runtime_error("no such room in the server to delete");
    }
}

};