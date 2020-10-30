#pragma once
#include <iostream>
#include <optional>

namespace bot{
/**
 * Useful class for logging stuff and defining logs out with cmake.
 * */
class logger{
protected:
    logger(){}
public:

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

    /**
     * Static function to get logger instance to save it as a reference.
     * Logger behaves like a singleton.
     * @returns refetence to logger instance.
     * */
    static auto& get_logger(){
        static std::optional<logger> lgr;
        if(!lgr.has_value()){
            lgr = logger();
        }
        return lgr.value();
    }
};

};