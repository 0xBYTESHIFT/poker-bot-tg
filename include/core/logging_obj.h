#pragma once

#include "components/logger.hpp"

namespace bot {
/** class that likes to write logs
     * */
class logging_obj {
protected:
    mutable logger m_lgr; /*< logger */
public:
    /**
     * Default constructor, initializes logger
     * */
    logging_obj();

    template<class Mes>
    auto desc(const Mes& mes) -> std::string;
};

logging_obj::logging_obj(): m_lgr(get_logger()) { }

template<class Mes>
auto logging_obj::desc(const Mes& mes) -> std::string {
    return fmt::format("{} {}[{}]", mes->chat->firstName, mes->chat->lastName, mes->chat->id);
}
} // namespace bot