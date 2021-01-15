#pragma once
#include "core/server.h"
#include "core/utils.h"

#include <memory>
#include <vector>

namespace poker {
using namespace bot;

class poker_server: public server {
public:
    poker_server();
    room_ptr create_room(user_ptr user) override;
};

poker_server::poker_server(): server() { }

room_ptr poker_server::create_room(user_ptr user) {
    auto prefix = fmt::format("poker_server::create_room user:{}", user->log_desc());
    lobby()->del_user(user);

    auto room     = std::make_shared<class poker::game_poker_room>(p_get_room_id());
    room->token() = token_generator::gen();
    room->add_user(user);
    room->owner()        = user;
    user->current_room() = room;
    rooms().emplace_back(room);

    m_lgr.info("{} created room {}", prefix, room->log_desc());
    return room;
}

}; // namespace poker