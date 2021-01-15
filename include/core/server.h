#pragma once
#include "components/logger.hpp"
#include "core/datatypes.h"
#include "core/logging_obj.h"
#include "core/property.h"
#include "core/room.h"
#include "core/user.h"
#include "core/utils.h"

#include <map>
#include <memory>
#include <random>
#include <set>
#include <string>
#include <vector>

namespace bot {
/**
 * Utility class to generate random tokens.
 * */
class token_generator {
    static inline std::set<std::string> p_tokens;   /**< Set to keep already generated tokens */
    const static size_t p_token_len            = 8; /**< Define for token's length */
    const static inline std::string p_alphabet =    /**< Alphabet for generating tokens */
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

public:
    /**
     * Func that generates a random unique token.
     * @returns a token
     * */
    static std::string gen();
};

std::string token_generator::gen() {
    static std::random_device rd;
    static std::mt19937 p_gen = std::mt19937(rd());
    static std::uniform_int_distribution<unsigned> dist =
        std::uniform_int_distribution<unsigned>(0, p_alphabet.size() - 1);

    std::string result(p_token_len, ' ');
    bool end = false;
    do {
        std::generate(result.begin(), result.end(), [&]() { return p_alphabet.at(dist(p_gen)); });
        if(p_tokens.find(result) == p_tokens.end()) {
            end = true;
            p_tokens.emplace(result);
        }
    } while(!end);
    return result;
};

/**
 * Server class to hold users and rooms
 * */
class server: public logging_obj {
    static inline id_t p_last_room_id = 0; /**< Last room id to keep generated room's unique */
protected:
    /**
     * Function to return new room's id
     * */
    id_t p_get_room_id();

public:
    using room_cont = std::vector<room_ptr>;                  /**< Define for rooms container */
    using user_cont = std::map<identifyable::id_t, user_ptr>; /**< Define for users container */

    property<room_cont> rooms = {};      /**< Rooms' pointers container */
    property<room_ptr> lobby  = nullptr; /**< Pointer to the lobby room */
    property<user_cont> users = {};      /**< Users' container */

    /**
     * Default constructor, initializes the lobby
     * */
    server();

    virtual ~server() {};
    /**
     * Function to find a user by their's id.
     * @returns user_ptr if they are found
     * */
    virtual user_ptr get_user(id_t id) const;
    /**
     * Function to find a room by it's token.
     * @returns room_ptr if it's found
     * */
    virtual room_ptr get_room(const room::token_t& token) const;
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

server::server() {
    lobby            = std::make_shared<room>(0);
    lobby()->name    = std::string("lobby");
    lobby()->token() = token_generator::gen();
}

id_t server::p_get_room_id() {
    return ++p_last_room_id;
}

user_ptr server::get_user(id_t id) const {
    auto prefix  = fmt::format("server::get_user id:{}", id);
    auto user_it = users().find(id);
    if(user_it != users().end()) {
        m_lgr.debug("{} was found", prefix);
        return user_it->second;
    }
    m_lgr.debug("{} wasn't found", prefix);
    return nullptr;
}

room_ptr server::get_room(const room::token_t& token) const {
    auto prefix  = fmt::format("server::get_room token:{}", token);
    auto room_it = std::find_if(rooms().begin(), rooms().end(), [&token](auto room) { return room->token() == token; });
    if(room_it != rooms().end()) {
        m_lgr.debug("{} was found", prefix);
        return *room_it;
    }
    m_lgr.debug("{} wasn't found", prefix);
    return nullptr;
}

room_ptr server::create_room(user_ptr user) {
    auto prefix = fmt::format("server::create_room {}", user->desc());
    lobby()->del_user(user);

    auto room     = std::make_shared<class room>(p_get_room_id());
    room->token() = token_generator::gen();
    room->add_user(user);
    room->owner()        = user;
    user->current_room() = room;
    rooms().emplace_back(room);

    m_lgr.info("{} created room {}", prefix, utils::get_desc(room));
    return room;
}

void server::on_user_connect(user_ptr user) {
    auto prefix = fmt::format("server::on_user_connect {}", user->desc());
    users().emplace(user->id, user);
    user->token = token_generator::gen();
    m_lgr.info("{} connected, got token:{}", prefix, user->token());
}

void server::on_user_disconnect(user_ptr user) {
    auto prefix = fmt::format("server::on_user_disconnect {}", user->desc());
    users().erase(user->id);
    m_lgr.info("{} diconnected", prefix);
}

void server::on_room_empty(room_ptr room) {
    auto prefix = fmt::format("server::on_room_empty room:{}", room->desc());
    if(!room->users().empty()) {
        m_lgr.error("{} called on non-empty room", prefix);
        return;
    }
    if(utils::erase(rooms(), room)) {
        m_lgr.info("{} removed a room", prefix);
    } else {
        auto mes = fmt::format("{} no such room in the server to delete", prefix);
        m_lgr.error(mes);
        throw std::runtime_error(mes);
    }
}

}; // namespace bot