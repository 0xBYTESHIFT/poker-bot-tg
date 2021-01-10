#pragma once
#include "core/property.h"
#include "poker/kinds.h"

#include <utility>

namespace poker {

class card {
protected:
public:
    unsigned value;
    struct kind kind;

    card(const card& k) = default;
    card(card&& k)      = default;
    card(unsigned value, const struct kind& k);
    card& operator=(const card& c) = default;
    bool operator==(const card& c)const;
    bool operator<(const card& c)const;
};

/*
card::card(const card& k): value(k.value), kind(k.kind) { }
card::card(card&& k): value(std::move(k.value)), kind(k.kind) { }
*/
card::card(unsigned value, const struct kind& k): value(value), kind(k) { }

bool card::operator==(const card& c) const {
    return this->value == c.value && this->kind == c.kind;
}
bool card::operator<(const card& c) const {
    return this->value < c.value;
}

}; // namespace poker