#pragma once
#include "core/datatypes.h"
#include "core/identifyable.h"
#include "core/lazy_utils.h"
#include "core/logger.h"
#include "core/logging_obj.h"
#include "core/nameable.h"
#include "core/property.h"
#include "core/user.h"

#include <memory>
#include <set>
#include <vector>

namespace bot {
/**
 * Room class to hold users and their information
 * */
class room: public nameable, public identifyable, public logging_obj {
public:
    using user_cont = std::vector<user_ptr>; /**< User container define */
    using token_t   = std::string;           /**< Room's token type define */
public:
    /**
     * Room's constructor.
     * @param id id to find a room in server's class and for convenient logging.
     * */
    room(id_t id);

    property<user_ptr> owner;            /**< Room's owner pointer. Only owner is allowed to kick/ban/mute users. */
    property<std::set<user_ptr>> banned, /**< Set with banned users to prevent them from joining. */
        muted,                           /**< Set with muted users to prevent them from writing. */
        unsubscribed;               /**< Set with unsubscribed users to prevent them from getting unwanted messages. */
    property<token_t> token   = ""; /**< Room's token, used for joining it. */
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
     * Procedure for determinig whether given user is in the room or not.
     * @param user ptr to user to check;
     * */
    virtual bool contains_user(const user_ptr user) const;
    /**
     * Procedure for get user from the room by it's id.
     * @param id id of the wanted user.
     * @returns user_ptr if it's found.
     * */
    virtual user_ptr get_user(const id_t& id) const;
    /**
     * Procedure for get user from the room by it's token.
     * @param token token of the wanted user.
     * @returns user_ptr if it's found.
     * */
    virtual user_ptr get_user(const user::token_t& token) const;
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
    virtual std::string desc() const;
    /**
     * Room's description, consists of name, tg ID and token.
     * @returns room's description, string
     * */
    virtual std::string log_desc() const;
};
}; // namespace bot

#include "core//utils.h"

namespace bot {

room::room(id_t id): identifyable(id) { }

void room::add_user(user_ptr user) {
    auto prefix = fmt::format("room::add_user room:{} {}[{}]", desc(), user->desc(), user->id());
    m_lgr.info("{} room:{}, adding user", prefix, desc());
    this->users().emplace_back(user);
}
void room::del_user(user_ptr user) {
    auto prefix = fmt::format("room::del_user room:{} {}[{}]", desc(), user->desc(), user->id());
    if(utils::erase(users(), user)) {
        user->current_room() = nullptr;
        m_lgr.info("{} room:{}, deleting user", prefix, desc());
    } else {
        auto mes = fmt::format("{} room:{} no such user in the room to delete", prefix, desc());
        m_lgr.error(mes);
        throw std::runtime_error(mes);
    }
}
bool room::contains_user(const user_ptr user) const {
    return bot::utils::contains(users.get(), user);
}
user_ptr room::get_user(const id_t& id) const {
    auto prefix  = fmt::format("room::get_user room:{} id:{}", desc(), id);
    auto pred    = [&id](const user_ptr& u) { return u->id() == id; };
    auto user_it = utils::find_if(users(), pred);
    if(user_it != users().end()) {
        m_lgr.debug("{} user was found", prefix);
        return *user_it;
    }
    m_lgr.debug("{} user wasn't found", prefix);
    return nullptr;
}
user_ptr room::get_user(const user::token_t& token) const {
    auto prefix  = fmt::format("room::get_user room:{} token:{}", desc(), token);
    auto pred    = [&token](const user_ptr& u) { return u->token() == token; };
    auto user_it = utils::find_if(users(), pred);
    if(user_it != users().end()) {
        m_lgr.debug("{} user was found", prefix);
        return *user_it;
    }
    m_lgr.debug("{} user wasn't found", prefix);
    return nullptr;
}

void room::process_mes(user_ptr user, mes_ptr mes) {
    auto prefix = fmt::format("room::process_mes room:{} user:{}", desc(), user->desc());
    m_lgr.debug("{} wrote: {}", prefix, mes->text);
}
std::string room::desc() const {
    return fmt::format("{}[{}]", name(), token());
}

std::string room::log_desc() const {
    return name() + "[tk:" + token() + "][id:" + std::to_string(id()) + "]";
}

}; // namespace bot

#include "core/user.h"
