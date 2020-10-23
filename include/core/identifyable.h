#pragma once
#include <cstddef>
#include "core/property.h"

namespace bot{
class identifyable{
public:
    using id_t = std::size_t;
protected:
    identifyable(id_t id):id(id){}
public:

    virtual ~identifyable(){}
    const property<id_t> id;
};

};