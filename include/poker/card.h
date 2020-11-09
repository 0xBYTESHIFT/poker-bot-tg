#pragma once
#include "core/property.h"
#include "poker/kinds.h"

#include <utility>

namespace poker {

class card {
protected:
public:
    const unsigned value;
    const class kind& kind;

    card(const card& k) = delete;
    card(card&& k);
    card(unsigned value, const class kind& k);
};

card::card(card&& k): value(std::move(k.value)), kind(k.kind) {}
card::card(unsigned value, const class kind& k): value(value), kind(k) {}

}; // namespace poker