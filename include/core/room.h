#pragma once
#include <memory>
#include <vector>
#include "core/nameable.h"
#include "core/identifyable.h"
#include "core/property.h"
#include "core/datatypes.h"
#include "core/logger.h"

namespace bot{
class room:public nameable, public identifyable{
public:
    using user_cont = std::vector<user_ptr>;
    using token_t = std::string;
protected:
    logger& lgr;
public:
    room(id_t id);

    property<user_ptr> owner;
    property<std::set<user_ptr>> banned, muted, unsubscribed;
    property<token_t> token = "";
    property<user_cont> users = {};

    virtual void add_user(user_ptr user);
    virtual void del_user(user_ptr user);
    virtual user_ptr get_user(const id_t &id)const;
    virtual user_ptr get_user(const user::token_t &token)const;
    virtual void process_mes(user_ptr user, mes_ptr mes);
    virtual std::string desc()const;
};
};

#include "core//utils.h"

namespace bot{

room::room(id_t id)
    :identifyable(id),
    lgr(logger::get_logger())
{}
void room::add_user(user_ptr user){
    this->users().emplace_back(user);
    lgr << "room:"<<desc()<<", user"<<bot::get_desc_log(user)<<" connected\n";
}
void room::del_user(user_ptr user){
    auto& users = this->users();
    auto user_it = std::find(users.begin(), users.end(), user);
    if(user_it != users.end()){
        users.erase(user_it);
        user->room() = nullptr;
        lgr << "room:"<<desc()<<", user"<<bot::get_desc_log(user)<<" disconnected\n";
    }
}
user_ptr room::get_user(const id_t &id)const{
    auto user_it = std::find_if(users().begin(), users().end(),
        [&id](auto u){ return u->id() == id; });
    if(user_it != users().end()){
        return *user_it;
    }
    return nullptr;
}
user_ptr room::get_user(const user::token_t &token)const{
    auto user_it = std::find_if(users().begin(), users().end(),
        [&token](auto u){ return u->token() == token; });
    if(user_it != users().end()){
        return *user_it;
    }
    return nullptr;
}

void room::process_mes(user_ptr user, mes_ptr mes){
    lgr << "room:"<<desc()<<", user"<<bot::get_desc_log(user)<<" wrote:"
        <<mes->text<<"\n";
}
std::string room::desc()const{
    return name()+"["+token()+"]";
}

};

#include "core/user.h"
