#pragma once
#include <string>
#include <functional>
#include "datatypes.h"

class command{
public:
    using name_t = std::string;
    using callback_t = std::function<void(mes_ptr)>;

    const name_t name;

    command(const name_t &name,
        const std::string &cmd_word,
        const std::vector<std::string> &args,
        const callback_t &callback);

    std::string usage()const;
    const std::string& cmd_word()const;
    const std::vector<std::string>& args()const;
    callback_t callback()const;
    void invoke(mes_ptr mes);
protected:
    const std::string m_cmd_word;
    const std::vector<std::string> m_args;
    const callback_t m_callback;
};

command::command(const name_t &name, const std::string &cmd_word,
    const std::vector<std::string> &args, const callback_t &callback)
    :name(name), m_cmd_word(cmd_word), m_args(args), m_callback(callback)
{}
std::string command::usage()const{
    std::string usage = "usage: "+m_cmd_word;
    for(auto &arg:m_args){
        usage+= " ["+arg+"]";
    }
    return usage;
}
const std::string& command::cmd_word()const{
    return m_cmd_word;
}
const std::vector<std::string>& command::args()const{
    return m_args;
}
command::callback_t command::callback()const{
    return m_callback;
}
void command::invoke(mes_ptr mes){
    m_callback(mes);
}