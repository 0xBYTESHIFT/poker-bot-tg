#pragma once
#include "core/property.h"

#include <cstddef>

namespace bot {
/**
 * Identifyable class interface.
 * */
class identifyable {
public:
    using id_t = std::size_t; /**< Id's type define */
protected:
    /**
     * Constructor, takes and stores an id.
     * @param id id to store.
     * */
    identifyable(id_t id): id(id) { }

public:
    /**
     * Virtual destructor.
     * Needed for polymorphism to work.
     * */
    virtual ~identifyable() { }
    const property<id_t> id; /**< Property that stores an id. */
};

}; // namespace bot