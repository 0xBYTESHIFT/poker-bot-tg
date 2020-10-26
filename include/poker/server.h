#pragma once
#include <memory>
#include <vector>
#include "core/server.h"

namespace poker{

class server_poker{
public:
    server_poker();
    virtual room_ptr create_room(user_ptr user);
};

server_poker::server_poker()
    :core::server()
{}

room_ptr server_poker::create_room(user_ptr user){
    lobby()->del_user(user);

    auto room = std::make_shared<class poker::room>(++p_last_room_id);
    room->token() = token_generator::gen();
    room->add_user(user);
    room->owner() = user;
    user->room() = room;
    rooms().emplace_back(room);

    lgr << "server: user"<<bot::get_desc_log(user)<<" created room"<<bot::get_desc(room)<<"\n";
}

};