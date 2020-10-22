#pragma once
#include <memory>
#include <vector>
#include "nameable.h"
#include "identifyable.h"
#include "property.h"
#include "datatypes.h"
#include "logger.h"

class room:public nameable, public identifyable{
public:
    using user_cont = std::vector<user_ptr>;

    logger& lgr;

    room(id_t id);
    property<user_cont> users = {};

    void add_user(user_ptr user);
    void del_user(user_ptr user);
    void process_mes(user_ptr user, mes_ptr mes);
};

room::room(id_t id)
    :identifyable(id),
    lgr(logger::get_logger())
{}
void room::add_user(user_ptr user){
    this->users().emplace_back(user);
    auto mes = "room:"+name()+", user["+std::to_string(user->id())+","+user->name()+"] connected\n";
    lgr.log(mes);
}
void room::del_user(user_ptr user){
    auto& users = this->users();
    auto user_it = std::find(users.begin(), users.end(), user);
    if(user_it != users.end()){
        users.erase(user_it);
        user->room() = nullptr;
    }
    auto mes = "room:"+name()+", user["+std::to_string(user->id())+","+user->name()+"] disconnected\n";
    lgr.log(mes);
}
void room::process_mes(user_ptr user, mes_ptr mes){
    auto log_mes = "room:"+name()+", user["+std::to_string(user->id())+","+user->name()+"] wrote:"+mes->text+"\n";
    lgr.log(log_mes);
}

#include "user.h"