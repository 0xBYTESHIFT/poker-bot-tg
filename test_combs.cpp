#include <execution>
#include <iostream>
#include <map>
#include <mutex>
#include <poker/card.h>
#include <poker/deck.h>
#include <poker/kinds.h>
#include <set>
#include <sstream>
#include <unordered_set>

using cards_t = std::vector<poker::card>;

/* ------------------------------------------------------------------------------------------------- */
// Produce a string whose lexicographic rank order reflects the ranking of the hand in poker.
// Examples:  "41000-63"     ;four of a kind (6s), the odd card is a 3
//            "22100-928"    ;two pair (9s and 2s), the odd card is an 8
//            "32000-da"     ;full house with three kings and two 10s
//            "312ST-ba987"  ;jack-high straight
// Hex digits (abcde) are used instead of TJQKA to get the lexicographic order right.
// Credit: geoffp at codewars
std::string ranking_string(const std::vector<poker::card>& cards) {
    unsigned c, r;

    // Count the number of occurrences of each rank:
    std::vector<unsigned> rc(15, 0);
    for(r = 2; r <= 14; r++) {
        for(auto& c: cards) {
            if(c.value == r) {
                ++rc[r];
            }
        }
    }

    // In an ace-low straight (but not in any other situation), an ace is considered to have rank 1:
    if(rc[2] == 1 && rc[3] == 1 && rc[4] == 1 && rc[5] == 1 && rc[14] == 1) {
        rc[1]  = 1;
        rc[14] = 0;
    }

    // Check for a straight:
    bool straight = std::search_n(rc.begin(), rc.end(), 5, 1) != rc.end();

    // Check for a flush:
    auto flush_pred = [&cards](auto c) { return c.kind == cards.front().kind; };
    bool flush             = std::all_of(cards.begin(), cards.end(), flush_pred);

    // Form the second (tie-breaking) part of the ranking string:
    std::string tiebreak;
    tiebreak.reserve(5 + 5 + 1);
    {
        std::ostringstream tiebreak_;
        std::ostringstream kinds_;
        for(c = 4; c > 0; c--) {
            for(r = 14; r >= 1; r--) {
                if(rc[r] == c) {
                    tiebreak_ << std::hex << r;
                    for(auto& card: cards) {
                        if(card.value == r || (r == 1 && card.value == 14)) {
                            kinds_ << card.kind.name.front();
                        }
                    }
                }
            }
        }
        tiebreak = tiebreak_.str();
        tiebreak += "-";
        tiebreak += kinds_.str();
    }

    // Form the first (hand type) part of the ranking string:
    std::string s;
    if(straight) {
        s = flush ? "5STFL" : "312ST";
    } else if(flush) {
        s = "313FL";
    } else {
        std::sort(rc.begin(), rc.end());
        for(r = 14; r > 9; r--) s += ('0' + rc[r]);
    }

    return s + "-" + tiebreak;
}

void print(const cards_t& cards) {
    for(auto& card: cards) {
        std::cout << "v:" << card.value;
        std::cout << " k:" << card.kind.name << "\n";
    }
    std::cout << "\n";
}

struct player {
    cards_t cards;
};

struct combination {
    int value;
    std::string name;
    std::vector<poker::card> cards = {};
    std::string comb_string        = "";

    combination(int value, const std::string& name): value(value), name(name) { }
    combination(): combination(-1, "") { }

    std::string dump() const {
        std::string result = name + " ";
        for(auto& card: cards) {
            result += std::to_string(card.value);
            result += card.kind.name.front();
            result += " ";
        }
        return result;
    }

    bool operator==(const combination& rhs) const {
        return value == rhs.value && name == rhs.name && std::equal(cards.begin(), cards.end(), rhs.cards.begin());
    }
    bool operator<(const combination& rhs) const { return comb_string < rhs.comb_string; }
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

template<class Str>
combination get_comb(Str&& comb_str, const cards_t& comb_cards) {
    combination comb;
    if(comb_str == "313FL") {
        comb = combs::flush;
    } else if(comb_str == "312ST") {
        comb = combs::straight;
    } else if(comb_str == "5STFL") {
        comb         = combs::straight_flush;
        auto beg     = comb_cards.begin();
        auto end     = comb_cards.end();
        bool has_ace = std::find_if(beg, end, [](auto c) { return c.value == 14; }) != end;
        bool has_ten = std::find_if(beg, end, [](auto c) { return c.value == 10; }) != end;
        if(has_ace && has_ten) {
            comb = combs::royal_flush;
        }
    } else if(comb_str.at(0) == '4') {
        comb = combs::four_of_a_kind;
    } else if(comb_str.at(0) == '3') {
        if(comb_str.at(1) == '2') {
            comb = combs::full_house;
        } else {
            comb = combs::three_of_a_kind;
        }
    } else if(comb_str.at(0) == '2') {
        if(comb_str.at(1) == '2') {
            comb = combs::pair_two;
        } else {
            comb = combs::pair_one;
        }
    } else {
        comb = combs::high;
    }
    return comb;
}

std::set<combination> get_combs(const cards_t& cards, const cards_t& hand) {
    std::set<combination> results;
    using hand_t = std::vector<poker::card>;
    hand_t combined;
    combined.reserve(cards.size() + hand.size());
    for(auto& c: cards) {
        combined.emplace_back(c);
    }
    for(auto& c: hand) {
        combined.emplace_back(c);
    }

    std::sort(combined.begin(), combined.end());

    std::set<hand_t> hands;

    do {
        hand_t tmp;
        tmp.reserve(5);
        std::copy(combined.begin(), combined.begin() + 5, std::back_inserter(tmp));
        std::sort(tmp.begin(), tmp.end());
        hands.emplace(std::move(tmp));
    } while(std::next_permutation(combined.begin(), combined.end()));

    std::vector<std::string> comb_strings;
    comb_strings.reserve(hands.size());
    for(auto& hand: hands) {
        auto tmp = ranking_string(hand);
        comb_strings.emplace_back(std::move(tmp));
    }
    //results.reserve(comb_strings.size());
    std::mutex mt;

    const size_t size = comb_strings.size();
    //std::for_each(std::execution::seq, comb_strings.begin(), comb_strings.end(), [&](const auto& str) {
    for(size_t i = 0; i < size; i++) {
        const auto& str = comb_strings.at(i);
        //std::cout << str << "\n";
        auto first_pos                 = str.find('-');
        auto last_pos                  = str.find_last_of('-');
        std::string_view comb_str      = std::string_view(str).substr(0, first_pos);
        std::string_view comb_str_full = std::string_view(str).substr(0, last_pos);

        std::vector<poker::card> comb_cards;
        {
            std::string str_ = str;
            int card_i       = 0;
            int str_i        = 0;
            int kind_i       = str.find_last_of('-') + 1;
            int val_i        = first_pos + 1;
            if(comb_str == "313FL" || comb_str == "312ST" || comb_str == "5STFL") {
                str_ = "11111" + str.substr(first_pos);
            }
            while(card_i < 5) {
                int comb_val = str_.at(str_i) - '0';
                while(comb_val > 0) {
                    auto hex_str   = str_.substr(val_i, 1);
                    int val_int    = std::stoi(hex_str, nullptr, 16);
                    char kind_char = str_.at(kind_i);
                    auto kind      = (kind_char == 't') ? poker::tiles :
                                     (kind_char == 'c') ? poker::clovers :
                                     (kind_char == 'p') ? poker::pikes :
                                                          poker::hearts;
                    comb_cards.emplace_back(val_int, kind);
                    comb_val--;
                    card_i++;
                    kind_i++;
                }
                str_i++;
                val_i++;
            }
            //prnt(comb_cards);

            auto comb        = get_comb(comb_str, comb_cards);
            comb.comb_string = comb_str_full;
            comb.cards       = std::move(comb_cards);
            {
                std::lock_guard<std::mutex> lock(mt);
                results.emplace(std::move(comb));
            }
        }
    }
    //);
    return results;
}

int main() {
    std::size_t repeats = 1000;
    std::map<std::string, std::size_t> cases;

    for(size_t i = 0; i < repeats; i++) {
        poker::deck d;
        if(i % 10 == 0) {
            std::cout << i * 1.0 / repeats * 100 << "%\n";
        }
        d.shuffle();

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

        //std::cout << "table:\n";
        //print(table);

        std::vector<std::set<combination>> combs;
        int pl_num = 0;
        for(auto& pl: plrs) {
            //std::cout << "player " << pl_num << "hand:\n";
            //std::cout << pl.cards.at(0).value << pl.cards.at(0).kind.name.front() << " ";
            //std::cout << pl.cards.at(1).value << pl.cards.at(1).kind.name.front() << "\n";
            auto pl_combs = get_combs(table, pl.cards);
            //std::cout << "player " << pl_num << " top 5 combs:\n";
            auto len = std::min(5LL, (long long)pl_combs.size());
            //auto len = pl_combs.size();
            //auto it = pl_combs.begin();
            //for(; it != pl_combs.begin() + len; it++) {
            for(auto& comb: pl_combs) {
                //std::cout << comb.dump() << "\n";
                cases[comb.name]++;
                --len;
            }
            //std::cout << "\n";
            combs.emplace_back(std::move(pl_combs));
            pl_num++;
        }
    }

    for(auto [name, count]: cases) {
        std::cout << name << " " << count * 1.0 / repeats << "\n";
    }

    return 0;
}