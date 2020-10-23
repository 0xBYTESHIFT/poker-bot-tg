#pragma once
#include <iostream>
#include <optional>

namespace bot{
class logger{
protected:
    logger(){}
public:

    template<class T>
    void log(const T& data) { 
        std::cout << data;
    }

    template<class T>
    logger& operator<<(const T& data) { 
        log(data);
        return *this;
    }

    static auto& get_logger(){
        static std::optional<logger> lgr;
        if(!lgr.has_value()){
            lgr = logger();
        }
        return lgr.value();
    }
};

};