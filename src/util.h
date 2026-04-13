#pragma once

#include <cstdint>
#include <string>

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
