#pragma once
#include "core/identifyable.h"
#include "core/nameable.h"

#include <cstddef>
#include <memory>

namespace bot {

class room;

/**
 * User's class to hold their's properties.
 * */
class user: public nameable, public identifyable {
public:
    using room_ptr = std::shared_ptr<room>; /**< define for room pointer */
    using token_t  = std::string;           /**< define for token's type */

    /**
     * User's constructor.
     * @param id id of a user, defined by Telegram
     * */
    user(id_t id);

    property<room_ptr> current_room = nullptr; /**< Room where user currently is */
    property<token_t> token =
        ""; /**< User's token to refer to them in commands */

    /**
     * User's description, consists of user's name and token.
     * @returns user's description
     * */
    virtual std::string desc() const;
};

user::user(id_t id): identifyable(id) {}

std::string user::desc() const {
    return name() + "[" + token() + "]";
}

}; // namespace bot

#include "core/room.h"