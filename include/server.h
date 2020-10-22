#pragma once
#include <memory>
#include <vector>
#include <map>
#include <string>
#include "property.h"
#include "room.h"
#include "user.h"
#include "datatypes.h"

class server{
public:
    using room_cont = std::vector<room_ptr>;
    using user_cont = std::map<identifyable::id_t, user_ptr>;

    property<room_cont> rooms = {};
    property<room_ptr> lobby = nullptr;
    property<user_cont> users = {};

    server();
    user_ptr get_user(id_t id)const;
};

server::server(){
    lobby = std::make_shared<room>(0);
    lobby()->name = std::string("lobby");
}

user_ptr server::get_user(id_t id)const{
    auto user_it = users().find(id);
    if(user_it != users().end()){
        return user_it->second;
    }
    return nullptr;
}