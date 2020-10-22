#pragma once
#include <string>
#include <memory>
#include "datatypes.h"

class user;
class room;

inline std::string get_desc_log(const user_ptr &user){
    std::string result = "[";
    result += std::to_string(user->id())+",";
    result += user->name()+"]";
    return result;
}

inline std::string get_desc(const user_ptr &user){
    return user->name()+"["+user->token()+"]";
}

inline std::string get_desc(const room_ptr &room){
    std::string result = "[";
    result += std::to_string(room->id())+",";
    result += room->token()+",";
    result += room->name()+"]";
    return result;
}

#include "room.h"
#include "user.h"