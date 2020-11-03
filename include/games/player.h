#pragma once
#include <memory>
#include "core/user.h"
#include "core/datatypes.h"
#include "core/property.h"

namespace games{

/**
 * Game player's class.
 * */
class player{
public:
    bot::property<bot::user_ptr> user; /**< Property storing bot's user pointer. */

    /**
     * Constructor.
     * @param bot's user pointer.
     * */
    player(bot::user_ptr user);
    /**
     * Virtual destructor for polymorphism purposes.
     * */
    virtual ~player();
};

player::player(bot::user_ptr user)
    :user(user)
{}
player::~player(){}
};