#include <iostream>
#include <poker/card.h>
#include <poker/deck.h>
#include <poker/kinds.h>

using cards_t = std::vector<std::unique_ptr<poker::card>>;

void print(const cards_t& cards) {
    for(auto& card: cards) {
        std::cout << "v:" << card->value;
        std::cout << " k:" << card->kind.name << std::endl;
    }
    std::cout << std::endl;
}

struct player {
    cards_t cards;
};

struct combination {
    int value;
    std::string name;
};

namespace combs {
static combination high {0, "high card"};
static combination pair_one {1, "pair"};
static combination pair_two {2, "two pair"};
static combination three_of_a_kind {3, "three of a kind"};
static combination straight {4, "straight"};
static combination flush {5, "flush"};
static combination full_house {6, "full house"};
static combination four_of_a_kind {7, "four of a kind"};
static combination straight_flush {8, "straight flush"};
static combination royal_flush {9, "royal flush"};
}; // namespace combs

std::vector<combination> get_combs(const cards_t& cards, const cards_t& hand) {
    std::vector<combination> results;
    std::vector<poker::card> combined;
    for(auto& c: cards) {
        combined.emplace_back(*c.get());
    }
    for(auto& c: hand) {
        combined.emplace_back(*c.get());
    }

    auto lbd = [](auto& c0, auto& c1) { return c0.value < c1.value; };
    std::sort(combined.begin(), combined.end(), lbd);

    auto prnt = [](auto cards) {
        for(auto& card: cards) {
            std::cout << "v:" << card.value;
            std::cout << " k:" << card.kind.name << std::endl;
        }
        std::cout << std::endl;
    };

    prnt(combined);
    std::vector<std::vector<poker::card>> hands;

    do {
        std::vector<poker::card> tmp;
        std::copy(combined.begin(), combined.begin() + 5,
                  std::back_inserter(tmp));
        if(std::find(hands.begin(), hands.end(), tmp) == hands.end()) {
            prnt(tmp);
            hands.emplace_back(std::move(tmp));
        }
    } while(std::next_permutation(combined.begin(), combined.end()));

    return results;
}

int main() {
    poker::deck d;
    d.shuffle();
    print(d.get_cards());

    const int players_size = 5;
    std::vector<player> plrs(players_size);
    for(auto& plr: plrs) {
        plr.cards.emplace_back(d.get_card());
        plr.cards.emplace_back(d.get_card());
    }
    cards_t table;
    for(size_t i = 0; i < 5; i++) {
        table.emplace_back(d.get_card());
    }
    print(table);
    get_combs(table, plrs.at(0).cards);

    return 0;
}