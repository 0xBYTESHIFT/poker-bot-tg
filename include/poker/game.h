#pragma once
#include "poker/bank.h"
#include "poker/deck.h"
#include "poker/coin.h"
#include "poker/player.h"
#include "core/property.h"
#include "core/datatypes.h"
#include "games/game.h"

namespace poker{
    
class game_poker:public games::game{
public:
    bot::property<class bank> bank;
    bot::property<class deck> deck;

    game_poker(const std::vector<bot::user_ptr> &users,
        const bank::init_vals_t &bank_vals
    );
};

game_poker::game_poker(const std::vector<bot::user_ptr> &users,
    const bank::init_vals_t &bank_vals)
    :games::game(),
    bank(bank_vals)
{}

};