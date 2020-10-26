#pragma once
#include <memory>
#include "core/user.h"
#include "core/datatypes.h"
#include "core/property.h"

namespace games{

class player{
public:
    bot::property<bot::user_ptr> user;

    player(bot::user_ptr user);
    virtual ~player();
};

player::player(bot::user_ptr user)
    :user(user)
{}
player::~player(){}
};