#pragma once
#include <memory>
#include <cstddef>
#include "identifyable.h"
#include "nameable.h"

class room;

class user:public nameable, public identifyable{
public:
    using room_ptr = std::shared_ptr<room>;

    user(id_t id):identifyable(id){}
    property<room_ptr> room = nullptr;
};

#include "room.h"