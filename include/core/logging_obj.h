#pragma once

#include "components/logger.hpp"

namespace bot {
/** class that likes to write logs
     * */
class logging_obj {
protected:
    logger m_lgr; /*< logger */
public:
    /**
     * Default constructor, initializes logger
     * */
    logging_obj();
};

logging_obj::logging_obj(): m_lgr(get_logger()) { }

} // namespace bot