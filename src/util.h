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

typedef uint8_t uint8;

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

    bool operator==(const Card& other) const { return rank == other.rank and suit == other.suit; }
    bool operator<(const Card& rhs) const { return rank < rhs.rank; }

    const std::string display() const {
        if (is_none()) return "   ";
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
    PileType source;
    PileType dest;
    uint8 source_pile;
    uint8 source_offset;
    uint8 dest_pile;
    uint8 dest_offset;
    bool reveal;

    static const Move draw(uint8 n) {
        return Move { .source = WASTE, .dest = WASTE, .source_offset = n };
    }
};

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

