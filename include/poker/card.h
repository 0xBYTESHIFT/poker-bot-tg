#pragma once
#include "core/property.h"
#include "poker/kinds.h"

#include <string>
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
    bool operator==(const card& c) const;
    bool operator<(const card& c) const;
};

card::card(unsigned value, const struct kind& k): value(value), kind(k) { }

bool card::operator==(const card& c) const {
    return this->value == c.value && this->kind == c.kind;
}
bool card::operator<(const card& c) const {
    std::string str   = std::to_string(this->value) + this->kind.name;
    std::string str_c = std::to_string(c.value) + c.kind.name;
    return str < str_c;
}

}; // namespace poker