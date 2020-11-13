#pragma once
#include "core/command.h"
#include "core/datatypes.h"
#include "core/room.h"
#include "core/server.h"
#include "core/user.h"
#include "core/utils.h"

#include <algorithm>
#include <optional>
#include <tgbot/tgbot.h>

#define ROOM_BOT_PREPARE(mes)                                                  \
    auto id   = mes->chat->id;                                                 \
    auto& s   = *this->s.get();                                                \
    auto tpl  = p_process_cmd(mes);                                            \
    auto user = std::get<0>(tpl);                                              \
    auto cmd  = std::get<1>(tpl);                                              \
    if(!user || !cmd) {                                                        \
        return;                                                                \
    }
namespace bot {
/**
 * Base bot class to do room-related stuff and basic social commands.
 * */
class room_bot {
protected:
    TgBot::Bot m_bot;          /**< Object to interact with Tg's api */
    std::unique_ptr<server> s; /**< Server ptr */
    const TgBot::Api&
        api; /**< Reference to the api, just for convenient access from within the class */

    /**
     * Function to react to start command \n
     * Adds user to server's lobby and server's users storage.
     * @param mes ptr to message from user
     * */
    void p_on_start(mes_ptr mes);
    /**
     * Function to react to stop command \n
     * Deletes user from their's room and from server's users storage.
     * @param mes ptr to message from user
     * */
    void p_on_stop(mes_ptr mes);
    /**
     * Function to react to any message \n
     * Resend's user message to other users in their's current room if it's not a command.
     * Commands are ignored here.
     * @param mes ptr to message from user
     * */
    void p_on_any(mes_ptr mes);

    /**
     * Function to react to room create request \n
     * Creates new room, then server class assigns random token to it and places it in server's rooms storage. \n
     * User that sent this request is placed into the new room and made into it's owner.  
     * @param mes ptr to message from user
     * */
    void p_on_room_create_request(mes_ptr mes);
    /**
     * Function to react to room close request \n
     * Removes user that sent the request from it's current room.
     * If they are the last person in this room, it will be deleted by server class.
     * User that sent this request is placed into the server's lobby room.
     * @param mes ptr to message from user
     * */
    void p_on_room_close_request(mes_ptr mes);
    /**
     * Function to react to room join request \n
     * Removes user that sent the request from it's current room and places them into requested room if it exists. \n
     * User has to specify room token that they want to join. User won't be joined if they are banned in the room. \n
     * It's impossible to close lobby.
     * @param mes ptr to message from user
     * */
    void p_on_room_join_request(mes_ptr mes);
    /**
     * Function to react to room list request \n
     * Sends list of users in request sender's current room and their user tokens. Also states if user is muted or not.
     * @param mes ptr to message from user
     * */
    void p_on_room_list_request(mes_ptr mes);
    /**
     * Function to react to room sunscribe request \n
     * Subscribes request sender to their current room's messages. User will recieve other users' messages.
     * @param mes ptr to message from user
     * */
    void p_on_room_subscribe_request(mes_ptr mes);
    /**
     * Function to react to room unsunscribe request \n
     * Unsubscribes request sender from their current room's messages. User will not recieve other users' messages.
     * @param mes ptr to message from user
     * */
    void p_on_room_unsubscribe_request(mes_ptr mes);
    /**
     * Function to react to room mute request \n
     * Mutes user that is specified by it's token in mute command. Command has to be sent by rooms' owner. \n
     * Muted user will not be able to send messages that are visible to other users. \n
     * It's impossible to mute yourself
     * @param mes ptr to message from user
     * */
    void p_on_room_mute_request(mes_ptr mes);
    /**
     * Function to react to room unmute request \n
     * Unmutes user that is specified by it's token in unmute command. Command has to be sent by rooms' owner. \n
     * Unmuted user will be able to send messages that are visible to other users. \n
     * @param mes ptr to message from user
     * */
    void p_on_room_unmute_request(mes_ptr mes);
    /**
     * Function to react to room ban request \n
     * Bans user that is specified by it's token in ban command. Command has to be sent by rooms' owner. \n
     * Banned user will be removed from the room and won't be able to join it again. \n
     * It's impossible to ban yourself
     * @param mes ptr to message from user
     * */
    void p_on_room_ban_request(mes_ptr mes);
    /**
     * Function to react to room unban request \n
     * Unbans user that is specified by it's token in ban command. Command has to be sent by rooms' owner. \n
     * Unbanned user will be able to join the room again.
     * @param mes ptr to message from user
     * */
    void p_on_room_unban_request(mes_ptr mes);
    /**
     * Function to react to room kick request \n
     * Kicks user that is specified by it's token in kick command. Command has to be sent by rooms' owner. \n
     * Kicked user will be able to join the room again. \n
     * It's impossible to kick yourself
     * @param mes ptr to message from user
     * */
    void p_on_room_kick_request(mes_ptr mes);

    std::vector<command>
        m_commands; /**< Commands storage. Useful to distinct command from regular message and to process it correctly */
    /**
     * Processes commands \n
     * Checks if sender is registered in the server. Checks if command is valid and has all required arguments. Otherwise sends it's correct usage to the sender. \n
     * Returns user ptr and command obj if command in the message is valid and user is known to the server. 
     * @param mes ptr to message from user
     * @returns user ptr and command obj;
     * */
    auto p_process_cmd(const mes_ptr& mes) const
        -> std::tuple<user_ptr, std::optional<command>>;

public:
    /**
     * Room bot's constructor \n
     * Inits TG API and default room-related commands.
     * @param token TG API token
     * */
    room_bot(const std::string& token);

    /**
     * Starts bot \n
     * Warning: takes current thread, don't expect this function to finish.
     * */
    void start();
};

void room_bot::p_on_start(mes_ptr mes) {
    auto id = mes->chat->id;
    api.sendMessage(id, "Hi!");
    auto& s       = *this->s.get();
    auto srv_user = s.get_user(id);
    if(srv_user) {
        return;
    } //prevent double joining
    auto user    = std::make_shared<class user>(id);
    user->name() = mes->chat->firstName;
    if(!mes->chat->lastName.empty()) {
        user->name() += " " + mes->chat->lastName;
    }

    s.lobby()->add_user(user);
    user->current_room() = s.lobby();
    s.on_user_connect(user);
}

void room_bot::p_on_stop(mes_ptr mes) {
    auto id   = mes->chat->id;
    auto& s   = *this->s.get();
    auto user = s.get_user(id);
    if(!user) {
        return;
    }
    user->current_room()->del_user(user);
    {
        auto& serv_users = s.users();
        serv_users.erase(id);
    }
}

void room_bot::p_on_any(mes_ptr mes) {
    auto id = mes->chat->id;
    auto& s = *this->s.get();

    for(auto& cmd: m_commands) {
        if(StringTools::startsWith(mes->text, "/" + cmd.cmd_word())) {
            return;
        } //skip if we got a command
    }
    auto user = s.get_user(id);
    if(!user) {
        return;
    }

    auto& room = user->current_room();
    room->process_mes(user, mes);
    if(room->muted().find(user) != room->muted().end()) {
        return;
    }

    std::string relay_mes = user->name() + ":" + mes->text;
    auto& users           = user->current_room()->users();
    for(const auto& u: users) {
        if(u == user) {
            continue;
        }
        api.sendMessage(u->id, relay_mes);
    }
}

void room_bot::p_on_room_create_request(mes_ptr mes) {
    ROOM_BOT_PREPARE(mes);

    auto room = s.create_room(user); //places user in that room too

    std::string response =
        "Welcome to new room,\n"
        "Send this token to your friends so they could join you:";
    api.sendMessage(id, response);
    response = room->token();
    api.sendMessage(id, response);
}

void room_bot::p_on_room_close_request(mes_ptr mes) {
    ROOM_BOT_PREPARE(mes);

    auto room = user->current_room();
    if(!room || room == s.lobby()) {
        return;
    } //don't delete null room or lobby
    room->del_user(user);
    if(room->users().empty()) {
        s.on_room_empty(room);
    }

    s.lobby()->add_user(user);
    user->current_room() = s.lobby();
    api.sendMessage(id, "Welcome to lobby!");
}
void room_bot::p_on_room_join_request(mes_ptr mes) {
    ROOM_BOT_PREPARE(mes);

    auto words = StringTools::split(mes->text, ' ');
    std::string response;
    const auto& token = words.at(1);
    auto room         = s.get_room(token);
    if(!room) {
        response = "No room with token " + token;
    } else if(room->banned().find(user) != room->banned().end()) {
        response = "You are banned from joining room " + room->desc();
    } else {
        response = "Welcome to room " + room->desc();
        user->current_room()->del_user(user); //delete from previous room
        room->add_user(user);         //place in joined room
        user->current_room() = room;          //save joined room in user too

        std::string broadcast_mes = "User " + user->desc() + " joined";
        for(auto& u: room->users()) {
            if(u == user) {
                continue;
            }
            api.sendMessage(id, broadcast_mes);
        }
    }
    api.sendMessage(id, response);
}
void room_bot::p_on_room_list_request(mes_ptr mes) {
    ROOM_BOT_PREPARE(mes);

    auto& room           = user->current_room();
    std::string response = "token name [muted]\n";
    for(auto& u: room->users()) {
        response += u->token() + " ";
        response += u->name() + " "; //"unnamed" if empty
        if(room->muted().find(u) != room->muted().end()) {
            response += "muted";
        }
        response += "\n";
    }
    api.sendMessage(id, response);
}
void room_bot::p_on_room_kick_request(mes_ptr mes) {
    ROOM_BOT_PREPARE(mes);

    auto words = StringTools::split(mes->text, ' ');
    auto& room = user->current_room();
    std::string response;

    if(room->owner() != user) {
        response = "You are not allowed to do this";
    } else {
        auto token       = words.at(1);
        auto user_kicked = room->get_user(token);
        if(!user_kicked) {
            response = "No user with token " + token + " in this room";
        } else if(user_kicked == user) {
            response = "You really should love yourself more, don't do this!";
        } else {
            room->del_user(user_kicked);      //remove user from kicked room
            s.lobby()->add_user(user_kicked); //place kicked user in lobby
            user_kicked->current_room() = s.lobby();  //save lobby as user's new room

            api.sendMessage(user_kicked->id,
                            "You were kicked from room " + room->desc());
            response = user_kicked->desc() + " was kicked from this room";
            for(auto& u: room->users()) {
                if(u == user) {
                    continue;
                }
                api.sendMessage(u->id, response);
            }
        }
    }
    api.sendMessage(id, response);
}
void room_bot::p_on_room_subscribe_request(mes_ptr mes) {
    ROOM_BOT_PREPARE(mes);

    auto words = StringTools::split(mes->text, ' ');
    auto& room = user->current_room();
    std::string response;

    if(room->unsubscribed().find(user) == room->unsubscribed().end()) {
        return;
    }
    room->unsubscribed().erase(user);
    response = "You've successfuly subscribed to room " + room->desc();
    api.sendMessage(id, response);
}
void room_bot::p_on_room_unsubscribe_request(mes_ptr mes) {
    ROOM_BOT_PREPARE(mes);

    auto words = StringTools::split(mes->text, ' ');
    auto& room = user->current_room();
    std::string response;

    if(room->unsubscribed().find(user) != room->unsubscribed().end()) {
        return;
    }
    room->unsubscribed().emplace(user);
    response = "You've successfuly unsubscribed from room " + room->desc() +
               "\n"
               "To subscribe back, use /sub command";
    api.sendMessage(id, response);
}
void room_bot::p_on_room_mute_request(mes_ptr mes) {
    ROOM_BOT_PREPARE(mes);

    auto words = StringTools::split(mes->text, ' ');
    auto& room = user->current_room();
    std::string response;

    if(room->owner() != user) {
        response = "You are not allowed to do this";
    } else {
        auto token      = words.at(1);
        auto user_muted = room->get_user(token);
        if(!user_muted) {
            response = "No user with token " + token + " in this room";
        } else if(user_muted == user) {
            response = "You shouldn't mute yourself";
        } else {
            room->muted().emplace(user_muted);

            api.sendMessage(user_muted->id,
                            "You were muted in room " + room->desc());
            response = user_muted->desc() + " was muted in this room";
            for(auto& u: room->users()) {
                if(u == user) {
                    continue;
                }
                api.sendMessage(u->id, response);
            }
        }
    }
    api.sendMessage(id, response);
}
void room_bot::p_on_room_unmute_request(mes_ptr mes) {
    ROOM_BOT_PREPARE(mes);

    auto words = StringTools::split(mes->text, ' ');
    auto& room = user->current_room();
    std::string response;

    if(room->owner() != user) {
        response = "You are not allowed to do this";
    } else {
        auto token = words.at(1);
        auto user_unmuted_it =
            std::find_if(room->muted().begin(), room->muted().end(),
                         [&token](auto u) { return u->token() == token; });
        if(user_unmuted_it == room->muted().end()) {
            response =
                "No user with token " + token + " was muted in this room";
        } else {
            auto user_unmuted = *user_unmuted_it;
            room->muted().erase(user_unmuted);

            api.sendMessage(user_unmuted->id,
                            "You were unmuted in room " + room->desc());
            response = user_unmuted->desc() + " was unmuted in this room";
            for(auto& u: room->users()) {
                if(u == user) {
                    continue;
                }
                api.sendMessage(u->id, response);
            }
        }
    }
    api.sendMessage(id, response);
}
void room_bot::p_on_room_ban_request(mes_ptr mes) {
    ROOM_BOT_PREPARE(mes);

    auto words = StringTools::split(mes->text, ' ');
    auto& room = user->current_room();
    std::string response;

    if(room->owner() != user) {
        response = "You are not allowed to do this";
    } else {
        auto token       = words.at(1);
        auto user_banned = room->get_user(token);
        if(!user_banned) {
            response = "No user with token " + token + " in this room";
        } else if(user_banned == user) {
            response = "Are you sure that you intended to ban yourself? I "
                       "can't allow this, sorry.";
        } else {
            room->banned().emplace(user_banned);
            room->del_user(user_banned);      //remove user from room
            s.lobby()->add_user(user_banned); //place user in lobby
            user_banned->current_room() = s.lobby();  //save lobby as user's new room

            api.sendMessage(user_banned->id,
                            "You were banned in room " + room->desc());
            response = user_banned->desc() + " was banned from this room";
            for(auto& u: room->users()) {
                if(u == user) {
                    continue;
                }
                api.sendMessage(u->id, response);
            }
        }
    }
    api.sendMessage(id, response);
}
void room_bot::p_on_room_unban_request(mes_ptr mes) {
    ROOM_BOT_PREPARE(mes);

    auto words = StringTools::split(mes->text, ' ');
    auto& room = user->current_room();
    std::string response;

    if(room->owner() != user) {
        response = "You are not allowed to do this";
    } else {
        auto token = words.at(1);
        auto user_unbanned_it =
            std::find_if(room->banned().begin(), room->banned().end(),
                         [&token](auto u) { return u->token() == token; });
        if(user_unbanned_it == room->banned().end()) {
            response =
                "No user with token " + token + " was banned in this room";
        } else {
            auto user_unbanned = *user_unbanned_it;
            room->banned().erase(user_unbanned);

            api.sendMessage(user_unbanned->id,
                            "You were unbanned in room " + room->desc());
            response = user_unbanned->desc() + " was unbanned in this room";
            for(auto& u: room->users()) {
                if(u == user) {
                    continue;
                }
                api.sendMessage(u->id, response);
            }
        }
    }
    api.sendMessage(id, response);
}

room_bot::room_bot(const std::string& token):
    api(m_bot.getApi()), m_bot(token) {
    this->s            = std::make_unique<server>();
    using args_t       = std::vector<std::string>;
    const auto no_args = args_t {};
    m_commands.emplace_back("start", "run this bot", no_args,
                            [this](auto mes) { p_on_start(mes); });
    m_commands.emplace_back("stop", "stop this bot", no_args,
                            [this](auto mes) { p_on_stop(mes); });
    m_commands.emplace_back(
        "create", "create a room", no_args,
        [this](auto mes) { p_on_room_create_request(mes); });
    m_commands.emplace_back("close", "close current room", no_args,
                            [this](auto mes) { p_on_room_close_request(mes); });
    m_commands.emplace_back("join", "join a room", args_t {"room_token"},
                            [this](auto mes) { p_on_room_join_request(mes); });
    m_commands.emplace_back("list", "list users in the room", no_args,
                            [this](auto mes) { p_on_room_list_request(mes); });
    m_commands.emplace_back("kick", "kick user", no_args,
                            [this](auto mes) { p_on_room_kick_request(mes); });
    m_commands.emplace_back("mute", "mute user", no_args,
                            [this](auto mes) { p_on_room_mute_request(mes); });
    m_commands.emplace_back(
        "unmute", "unmute user", args_t {"user_token"},
        [this](auto mes) { p_on_room_unmute_request(mes); });
    m_commands.emplace_back("ban", "ban user", args_t {"user_token"},
                            [this](auto mes) { p_on_room_ban_request(mes); });
    m_commands.emplace_back("unban", "unban user", args_t {"user_token"},
                            [this](auto mes) { p_on_room_unban_request(mes); });
    m_commands.emplace_back(
        "sub", "subscribe back to room's messages", no_args,
        [this](auto mes) { p_on_room_subscribe_request(mes); });
    m_commands.emplace_back(
        "unsub", "unsubscribe from room's messages", no_args,
        [this](auto mes) { p_on_room_unsubscribe_request(mes); });
    auto& ev = m_bot.getEvents();
    for(auto& cmd: m_commands) {
        ev.onCommand(cmd.cmd_word(), cmd.callback());
    }
    ev.onAnyMessage([this](mes_ptr mes) { p_on_any(mes); });
}

auto room_bot::p_process_cmd(const mes_ptr& mes) const
    -> std::tuple<user_ptr, std::optional<command>> {
    auto id   = mes->chat->id;
    auto& s   = *this->s.get();
    auto user = s.get_user(id);
    if(!user) {
        return std::make_tuple(user, std::nullopt);
    }
    auto words = StringTools::split(mes->text, ' ');
    auto cmd_it =
        std::find_if(m_commands.begin(), m_commands.end(), [&](auto cmd) {
            return "/" + cmd.cmd_word() == words.at(0);
        });
    if(cmd_it == m_commands.end()) {
        api.sendMessage(id, "Unknown command");
        return std::make_tuple(user, std::nullopt);
    }
    auto cmd = *cmd_it;
    if(words.size() - 1 != cmd.args().size()) {
        api.sendMessage(id, cmd.usage());
        return std::make_tuple(user, std::nullopt);
    }
    return std::make_tuple(user, std::move(cmd));
}

void room_bot::start() {
    printf("Bot username: %s\n", m_bot.getApi().getMe()->username.c_str());
    TgBot::TgLongPoll longPoll(m_bot);
    while(true) {
        longPoll.start();
    }
}

} // namespace bot