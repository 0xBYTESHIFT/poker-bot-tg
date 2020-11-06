#pragma once
#include "core/datatypes.h"

#include <functional>
#include <string>

namespace bot {

/**
 * Bot's command class.
 * */
class command {
public:
    using name_t = std::string; /**< Name type define. */
    using callback_t =
        std::function<void(mes_ptr)>; /**< Callback type define. */

    /**
     * Command's constructor.
     * @param name name of the command.
     * @param desc description of the command.
     * @param args vector for command's arguments names.
     * @param callback callback to be called when command is entered correctly.
     * */
    command(const name_t& name, const std::string& desc,
            const std::vector<std::string>& args, const callback_t& callback);

    /**
     * Returns info about using a command. Example: "/kick [user_token]".
     * @returns string with info about command's usage.
     * */
    auto usage() const -> std::string;
    /**
     * Returns name of a command. Example: "kick".
     * @returns string with the name of a command.
     * */
    auto cmd_word() const -> const std::string&;
    /**
     * Returns description of a command. Example: "kicks unwanted player out".
     * @returns string with the description of a command.
     * */
    auto desc() const -> const std::string&;
    /**
     * Returns vector of command's arguments names. Example: {"user_token",}.
     * @returns vector of strings, representing command's arguments names.
     * */
    auto args() const -> const std::vector<std::string>&;
    /**
     * Returns callback of a command.
     * @returns callback of a command.
     * */
    auto callback() const -> callback_t;
    /**
     * Call's command's callback with user's message.
     * @param mes message from user with command's call.
     * */
    void invoke(mes_ptr mes);

protected:
    const std::string m_desc;              /**< Description of a command. */
    const std::string m_cmd_word;          /**< Command's name. */
    const std::vector<std::string> m_args; /**< Command's args names. */
    const callback_t m_callback;           /**< Command's callback. */
};

command::command(const name_t& name, const std::string& desc,
                 const std::vector<std::string>& args,
                 const callback_t& callback):
    m_cmd_word(name),
    m_desc(desc), m_args(args), m_callback(callback) {}
auto command::usage() const -> std::string {
    std::string usage = "Usage: /" + m_cmd_word;
    for(auto& arg: m_args) {
        usage += " [" + arg + "]";
    }
    return usage;
}
auto command::cmd_word() const -> const std::string& {
    return m_cmd_word;
}
auto command::desc() const -> const std::string& {
    return m_desc;
}
auto command::args() const -> const std::vector<std::string>& {
    return m_args;
}
auto command::callback() const -> command::callback_t {
    return m_callback;
}
void command::invoke(mes_ptr mes) {
    m_callback(mes);
}

}; // namespace bot