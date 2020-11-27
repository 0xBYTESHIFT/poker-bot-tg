#pragma once
#include <string>

namespace poker {

enum class cards_color { red, black };

struct kind {
    const size_t id;
    const std::string name;
    const cards_color color;

    virtual ~kind() {};

    kind(const kind& rhs) = delete;
    kind(size_t id, const std::string& name, const cards_color& color);
};

kind::kind(size_t id, const std::string& name, const cards_color& color):
    id(id), name(name), color(color) {}

static inline const kind hearts(0, "hearts", cards_color::red);
static inline const kind tiles(1, "tiles", cards_color::red);
static inline const kind clovers(2, "clovers", cards_color::black);
static inline const kind pikes(3, "pikes", cards_color::black);

}; // namespace poker