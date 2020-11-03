#pragma once
#include <vector>
#include "games/player.h"
#include "core/property.h"

namespace games{

/**
 * Class to be inherited for games classes.
 * */
class game{
public:
    using players_cont = std::vector<player>; /**< Define for player's container. */

    bot::property<players_cont> players; /**< Property storing players. */

    /**
     * Default constructor.
     * */
    game();
    /**
     * Virtual destructor for polymorphism purposes.
     * */
    virtual ~game();
};

game::game(){}
game::~game(){};

};