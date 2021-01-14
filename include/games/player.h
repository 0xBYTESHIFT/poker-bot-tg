#pragma once
#include "core/datatypes.h"
#include "core/property.h"
#include "core/user.h"

#include <memory>
#include <queue>

namespace games {

/**
 * Game player's class.
 * */
class player {
protected:
    std::queue<std::string> mes_to_send; /**< Messages queue to send to user. */
public:
    bot::property<bot::user_ptr> user; /**< Property storing bot's user pointer. */

    /**
     * Constructor.
     * @param user bot's user pointer.
     * */
    player(bot::user_ptr user);
    /**
     * Virtual destructor for polymorphism purposes.
     * */
    virtual ~player();

    /**
     * Comparison operator.
     * @param rhs right parameter to compare. 
     * @param lhs left parameter to compare. 
     * @returns true if parameters are equal, false of not.
     * */
    friend bool operator==(const player& lhs, const player& rhs);

    /**
     * Function to add message to send to player later.
     * @param mes message to send.
     * */
    void send(const std::string& mes);

    /**
     * Function to get message queue to send to player.
     * @returns messages queue that has to be sent to user.
     * */
    auto mes_queue() -> std::queue<std::string>&;
};

player::player(bot::user_ptr user): user(user) { }
player::~player() { }

bool operator==(const player& lhs, const player& rhs) {
    return lhs.user() == rhs.user();
}

void player::send(const std::string& mes) {
    mes_to_send.emplace(mes);
}

auto player::mes_queue() -> std::queue<std::string>& {
    return mes_to_send;
}

}; // namespace games