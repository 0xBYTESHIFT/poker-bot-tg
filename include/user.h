#pragma once
#include <memory>
#include <cstddef>
#include "identifyable.h"
#include "nameable.h"

class room;

class user:public nameable, public identifyable{
public:
    using room_ptr = std::shared_ptr<room>;
    using token_t = std::string;

    user(id_t id):identifyable(id){}

    property<room_ptr> room = nullptr;
    property<token_t> token = "";

    std::string desc()const;
};

std::string user::desc()const{
    return name()+"["+token()+"]";
}
#include "room.h"