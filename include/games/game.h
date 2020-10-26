#pragma once
#include <vector>
#include "games/player.h"
#include "core/property.h"

namespace games{

class game{
public:
    using players_cont = std::vector<player>;

    bot::property<players_cont> players;

    game();
    virtual ~game();
};

game::game(){}
game::~game(){};

};