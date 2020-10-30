#pragma once
#include <optional>
#include <memory>

namespace patterns{

template<class T>
class singleton{
public:
    singleton(const singleton& rhs) = delete;
    singleton& opetator(const singleton& rhs) = delete;

    static auto& get_instance(){
        static std::unique_ptr<T> instance;
        if(!instance){
            instance = std::make_unique<T>(singleton_token{});
        }
        return *instance;
    }
protected:
    struct singleton_token {};
    singleton()=default;
};

};