#pragma once
#include <string>
#include <functional>
#include "core/datatypes.h"

namespace bot{

class command{
public:
    using name_t = std::string;
    using callback_t = std::function<void(mes_ptr)>;

    command(const name_t &name,
        const std::string &desc,
        const std::vector<std::string> &args,
        const callback_t &callback);

    auto usage()const -> std::string;
    auto cmd_word()const -> const std::string&;
    auto desc()const -> const std::string&;
    auto args()const -> const std::vector<std::string>&;
    auto callback()const -> callback_t;
    void invoke(mes_ptr mes);
protected:
    const std::string m_desc;
    const std::string m_cmd_word;
    const std::vector<std::string> m_args;
    const callback_t m_callback;
};

command::command(const name_t &name, const std::string &desc,
    const std::vector<std::string> &args, const callback_t &callback)
    :m_cmd_word(name), m_desc(desc), m_args(args), m_callback(callback)
{}
auto command::usage()const -> std::string{
    std::string usage = "Usage: /"+m_cmd_word;
    for(auto &arg:m_args){
        usage+= " ["+arg+"]";
    }
    return usage;
}
auto command::cmd_word()const -> const std::string&{
    return m_cmd_word;
}
auto command::desc()const -> const std::string&{
    return m_desc;
}
auto command::args()const -> const std::vector<std::string>&{
    return m_args;
}
auto command::callback()const -> command::callback_t{
    return m_callback;
}
void command::invoke(mes_ptr mes){
    m_callback(mes);
}

};