#pragma once
#include "patterns/singleton.h"

#include <iostream>
#include <optional>

namespace bot {
/**
 * Useful class for logging stuff and defining logs out with cmake.
 * */
class logger: public patterns::singleton<logger> {
    using base =
        patterns::singleton<logger>; /**< define for convenient usage */
public:
    /**
     * Default constructor. May be called only from singleton base class
     * due to sinleton_token being protected in it.
     * @param token token to prevent user constructing of logger.
     * */
    logger(base::singleton_token token) {}

    /**
     * Log any templated parameter to cout
     * @param data templated parameter
     * */
    template<class T>
    void log(const T& data) {
        std::cout << data;
    }

    /**
     * Log any templated parameter to cout.
     * @param data templated parameter
     * @returns reference to logger instance.
     * */
    template<class T>
    logger& operator<<(const T& data) {
        log(data);
        return *this;
    }
};

}; // namespace bot