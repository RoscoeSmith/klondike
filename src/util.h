#pragma once

#include "ansi.h"
#include <array>
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <sstream>
#include <exception>
#include <random>
#include <utility>

typedef uint8_t uint8;

std::ostream& operator<<(std::ostream& os, const uint8& i) {
    os << static_cast<int>(i);
    return os;
}

typedef std::pair<int, int> int_pair;

// initialize RNG
#define RNG_SEED 0
static auto RNG = std::mt19937(RNG_SEED);

enum Suit {
    SPADE = 0,
    HEART,
    CLUB,
    DIAMOND,
    NO_SUIT
};

constexpr Suit operator++(Suit& s) {
    switch (s) {
        case SPADE:
            s = HEART;
            break;
        case HEART:
            s = CLUB;
            break;
        case CLUB:
            s = DIAMOND;
            break;
        case DIAMOND:
            s = NO_SUIT;
            break;
        case NO_SUIT:
            throw std::range_error("Tried to increment NO_SUIT");
            s = NO_SUIT;
            break;
    }
    return s;
}

static std::array<std::string, 14> rank_to_char = {
    "x ", "A ", "2 ", "3 ", "4 ", "5 ", "6 ", "7 ", "8 ", "9 ", "10", "J ", "Q ", "K "
};

static std::array<std::string, 5> suit_to_char = {
    "♠", "♥", "♣", "♦", "x"
};

static std::array<std::string, 5> suit_to_ansi = {
    ANSI_CARD_FG_BLACK, ANSI_CARD_FG_RED, ANSI_CARD_FG_GREEN, ANSI_CARD_FG_BLUE, ""
};

struct Card {
    uint8 rank;
    Suit suit;
    bool face_up;

    static const Card NONE;

    const bool is_none() const { return *this == Card::NONE; }
    const bool is_black() const { return suit == SPADE or suit == CLUB; }
    const bool is_red() const { return suit == DIAMOND or suit == HEART; }

    // basic comparison
    bool operator==(const Card& other) const { return rank == other.rank and suit == other.suit; }
    bool operator<(const Card& rhs) const { return rank < rhs.rank; }

    // card on lhs goes on top of rhs in foundation sequence
    bool operator^(const Card& rhs) const {
        if (rank == 1) return rhs.is_none();
        else return suit == rhs.suit and rank == rhs.rank + 1;
    }

    // card on lhs goes on top of rhs in tableau sequence
    bool operator>>(const Card& rhs) const {
        if (rank == 13) return rhs.is_none();
        else return !is_none() and rank == rhs.rank - 1 and ((is_black() and rhs.is_red()) or (is_red() and rhs.is_black()));
    }
    bool operator<<(const Card& rhs) const {
        return rhs >> *this;
    }

    const std::string display() const {
        if (is_none()) return "[   ]";
        std::stringstream out;
        if (face_up) {
            out << ANSI_CARD_FG_WHITE << "▐" << suit_to_ansi[suit] << ANSI_CARD_BG_FACEUP << rank_to_char[rank] << suit_to_char[suit] << ANSI_RESET << ANSI_CARD_FG_WHITE << "▌" << ANSI_RESET;
        } else {
            if (false) {  // to be used for thoughtful mode
                out << ANSI_CARD_FG_DULLWHITE << "▐" << suit_to_ansi[suit] << ANSI_CARD_BG_FACEUP_THOUGHTFUL << rank_to_char[rank] << suit_to_char[suit] << ANSI_RESET << ANSI_CARD_FG_DULLWHITE << "▌" << ANSI_RESET;
            } else {
                out << ANSI_CARD_FG_WHITE << "▐" << ANSI_CARD_BG_FACEDOWN << ANSI_CARD_FG_WHITE << "▒" << ANSI_CARD_BG_FACEDOWN_HARSH << "░" << ANSI_CARD_BG_FACEDOWN << "▒" << ANSI_RESET << ANSI_CARD_FG_WHITE << "▌" << ANSI_RESET;
            }
        }
        return out.str();
    }

    const std::string debug_display() const {
        if (is_none()) return ".";
        std::stringstream out;
        out << rank_to_char[rank] << suit_to_char[suit] << (face_up ? "" : "?");
        return out.str();
    }

    const std::string get_tag(const bool ansi = false) const {
        std::stringstream out;

        if (is_none()) {
            if (ansi) {
                out << ANSI_CARD_BG_FACEUP_THOUGHTFUL << ANSI_CARD_FG_BLACK << "--" << ANSI_RESET;
            } else {
                out << "--";
            }
        } else {
            if (ansi) {
                if (false) {  // to be used for thoughtful mode
                    out << ANSI_CARD_BG_FACEUP_THOUGHTFUL;
                } else {
                    out << ANSI_CARD_BG_FACEUP;
                }
                out << suit_to_ansi[suit];
            }
            std::string r = rank_to_char[rank];
            if (rank != 10) r = r.substr(0, 1);
            out << r << suit_to_char[suit];
            if (ansi) {
                out << ANSI_RESET;
            }
        }

        return out.str();
    }
};

std::ostream& operator<<(std::ostream& os, const Card& card) {
    // os << rank_to_char[card.rank] << suit_to_char[card.suit];
    os << card.display();
    return os;
}

const Card Card::NONE = { .rank = 0, .suit = NO_SUIT };

enum PileType {
    TABLEAU = 0,
    FOUNDATION,
    WASTE
};

// move is draw if both source and dest are WASTE
struct Move {
    const PileType source;
    const PileType dest;
    const int source_pile;
    const int source_offset;  // for draw moves, this encodes no. of cards drawn
    const int dest_pile;
    const int dest_offset;
    const bool extra;  // for draw moves, this represents if the stock was recycled

    Move(const PileType source, const PileType dest, const int source_pile, const int source_offset, const int dest_pile, const int dest_offset, const bool extra)
        : source(source), dest(dest), source_pile(source_pile), source_offset(source_offset), dest_pile(dest_pile), dest_offset(dest_offset), extra(extra) {}
        
    static const Move draw(const uint8 n, const bool recycle) {
        return Move(WASTE, WASTE, -1, n, -1, -1, recycle);
    }

    virtual const std::string display() const {
        std::stringstream out;
        if (source == WASTE and dest == WASTE) {
            if (extra) out << "recycle, ";
            out << "draw " << source_offset;
        } else {
            std::string source_str;
            std::string dest_str;
            switch (source) {
                case TABLEAU:
                    source_str = "tableau";
                    break;
                case FOUNDATION:
                    source_str = "foundation";
                    break;
                case WASTE:
                    source_str = "waste";
                    break;
            }
            switch (dest) {
                case TABLEAU:
                    dest_str = "tableau";
                    break;
                case FOUNDATION:
                    dest_str = "foundation";
                    break;
                case WASTE:
                    dest_str = "waste";
                    break;
            }
            out << source_str << " " << source_pile << "," << source_offset << " to " << dest_str << " " << dest_pile << "," << dest_offset;
            if (extra) out << ", reveal";
        }
        return out.str();
    }
};

std::ostream& operator<<(std::ostream& os, const Move& move) {
    os << move.display();
    return os;
}

// util for decks of 52 cards
typedef std::array<Card, 52> Deck;

constexpr Deck unshuffled_deck() {
    Deck deck {};
    for (Suit s = SPADE; s != NO_SUIT; ++s) {
        for (uint8 r = 1; r < 14; ++r) {
            deck[13 * static_cast<uint8>(s) + (r - 1)] = Card{ .rank = r, .suit = Suit(s), .face_up = false };
        }
    }
    return deck;
}

static constexpr Deck UNSHUFFLED_DECK = unshuffled_deck();

Deck shuffled_deck() {
    Deck deck = UNSHUFFLED_DECK;
    std::shuffle(deck.begin(), deck.end(), RNG);
    return deck;
}

