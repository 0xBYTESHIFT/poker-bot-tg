#pragma once
#include <memory>
#include <optional>

namespace patterns {

/**
 * Singleton's pattern class.
 * */
template<class T>
class singleton {
public:
    /**
     * Deleted copy constructor.
     * @param rhs singleton to copy.
     * */
    singleton(const singleton& rhs) = delete;
    /**
     * Deleted assignment operator.
     * @param rhs singleton to assign.
     * */
    singleton& opetator(const singleton& rhs) = delete;

    /**
     * Funtion to get instance. Constructs it if it's not constructed yet.
     * @returns refetence to T's instance.
     * */
    static auto& get_instance() {
        static std::unique_ptr<T> instance;
        if(!instance) {
            instance = std::make_unique<T>(singleton_token {});
        }
        return *instance;
    }

protected:
    /**
     * Token to prevent derived classes to be constructed not like a singleton.
     * */
    struct singleton_token { };
    /**
     * Default constructor for derived classes to use.
     * */
    singleton() = default;
};

}; // namespace patterns