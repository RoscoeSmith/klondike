#pragma once

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

struct Card {
    uint8 rank;
    Suit suit;
    bool face_up;

    static const Card NONE;

    const bool is_none() const { return *this == Card::NONE; }

    bool operator==(const Card& other) const { return rank == other.rank and suit == other.suit; }
    bool operator<(const Card& rhs) const { return rank < rhs.rank; }

    std::string debug_display() const {
        if (is_none()) return ".";
        std::stringstream out;
        out << static_cast<int>(rank) << "," << static_cast<int>(suit) << (face_up ? "" : "?");
        return out.str();
    }
};

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

