#pragma once
#include <string>
#include "property.h"

class nameable{
public:
    using name_t = std::string;
protected:
    nameable(const name_t name = "unnamed")
        :name(name){}
public:

    virtual ~nameable(){}
    property<name_t> name;
};
