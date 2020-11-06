#pragma once
#include "core/datatypes.h"

#include <memory>
#include <string>

namespace bot {

class user;
class room;

namespace utils {

inline std::string get_desc_log(const user_ptr& user) {
    std::string result = "[";
    result += std::to_string(user->id()) + ",";
    result += user->name() + "]";
    return result;
}

inline std::string get_desc(const room_ptr& room) {
    std::string result = "[";
    result += std::to_string(room->id()) + ",";
    result += room->token() + ",";
    result += room->name() + "]";
    return result;
}

inline std::string get_desc(const user_ptr& user) {
    return user->name() + "[" + user->token() + "]";
}

} // namespace utils

}; // namespace bot

#include "core/room.h"
#include "core/user.h"