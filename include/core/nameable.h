#pragma once
#include "core/property.h"

#include <string>

namespace bot {

/**
 * Class to hold name of derived entities.
 * */
class nameable {
public:
    using name_t = std::string; /**< Name type define */
protected:
    /**
     * Nameable constructor.
     * @param name name to save, default = "unnamed"
     * */
    nameable(const name_t name = "unnamed");

public:
    /**
     * Virtual destructor
     * */
    virtual ~nameable();
    property<name_t> name; /**< Name of derived class object */
};

nameable::nameable(const name_t name): name(name) {}
nameable::~nameable() {}

}; // namespace bot