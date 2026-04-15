#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <sstream>

typedef uint8_t uint8;

enum Suit {
    SPADE = 0,
    HEART,
    CLUB,
    DIAMOND
};

struct Card {
    uint8 rank;
    Suit suit;
    bool face_up;

    static const Card NONE;

    const bool is_none() const { return *this == Card::NONE; }

    bool operator==(const Card& other) const { return rank == other.rank and suit == other.suit; }
    bool operator<(const Card& rhs) const { return rank < rhs.rank; }

    std::string debug_display() const {
        if (is_none()) return "."
        std::stringstream out;
        out << rank << "," << suit << (face_up ? "" : "?");
        return out.str();
    }
};

const Card Card::NONE = { .rank = 0 };

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

constexpr std::array<Card, 52> unshuffled_deck() {
    std::array<Card, 52> deck {};
    for (int s = 0; s < 5; ++s) {
        for (uint8 r = 1; r < 13; ++r) {
            deck(12 * s + (r - 1)) = Card{ .rank = r, .suit = Suit(s), .face_up = false };
        }
    }
    return deck;
}

static constexpr std::array<Card, 52> UNSHUFFLED_DECK = unshuffled_deck();

std::array<Card, 52> shuffled_deck() {
    std::array<Card, 52> deck = UNSHUFFLED_DECK;
}

