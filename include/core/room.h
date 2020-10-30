#pragma once
#include <memory>
#include <vector>
#include "core/nameable.h"
#include "core/identifyable.h"
#include "core/property.h"
#include "core/datatypes.h"
#include "core/logger.h"

namespace bot{
/**
 * Room class to hold users and their information
 * */
class room:public nameable, public identifyable{
public:
    using user_cont = std::vector<user_ptr>; /**< User container define */
    using token_t = std::string; /**< Room's token type define */
protected:
    logger& lgr; /**< logger reference, since it's a singleton */
public:
    /**
     * Room's constructor.
     * @param id id to find a room in server's class and for convenient logging.
     * */
    room(id_t id);

    property<user_ptr> owner; /**< Room's owner pointer. Only owner is allowed to kick/ban/mute users. */
    property<std::set<user_ptr>> banned, /**< Set with banned users to prevent them from joining. */
        muted, /**< Set with muted users to prevent them from writing. */
        unsubscribed; /**< Set with unsubscribed users to prevent them from getting unwanted messages. */
    property<token_t> token = ""; /**< Room's token, used for joining it. */
    property<user_cont> users = {}; /**< Users' container. */

    /**
     * Procedure for adding user into the room.
     * @param user ptr to user to join
     * */
    virtual void add_user(user_ptr user);
    /**
     * Procedure for deleting user from the room.
     * @param user ptr to user to delete
     * */
    virtual void del_user(user_ptr user);
    /**
     * Procedure for get user from the room by it's id.
     * @param id id of the wanted user.
     * @returns user_ptr if it's found.
     * */
    virtual user_ptr get_user(const id_t &id)const;
    /**
     * Procedure for get user from the room by it's token.
     * @param token token of the wanted user.
     * @returns user_ptr if it's found.
     * */
    virtual user_ptr get_user(const user::token_t &token)const;
    /**
     * Procedure for processing user message, basically just logs it for now.
     * @param user ptr to the sender
     * @param mes ptr to the message
     * */
    virtual void process_mes(user_ptr user, mes_ptr mes);
    /**
     * Room's description, consists of name and token.
     * @returns room's description, string
     * */
    virtual std::string desc()const;
};
};

#include "core//utils.h"

namespace bot{

room::room(id_t id)
    :identifyable(id),
    lgr(logger::get_instance())
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
