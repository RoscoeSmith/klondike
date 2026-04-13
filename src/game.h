#pragma once

#include "util.h"
#include <array>
#include <span>
#include <stdexcept>

using std::array, std::span;

const size_t TABLEAU_SIZE = 19;
const size_t FOUNDATION_SIZE = 13;
const size_t STOCK_SIZE = 24;

struct Klondike {
    struct State {
        array<array<Card, TABLEAU_SIZE>, 7> tableau;
        array<array<Card, FOUNDATION_SIZE>, 4> foundation;
        array<Card, STOCK_SIZE> stock;
        uint waste_cap;

        State deal(array<Card, 52>& cards) {
            // default-init arrays
            tableau = { Card::NONE };
            foundation = { Card::NONE };
            
            int c = 0;  // card counter

            // deal tableau
            for (int pile = 1; pile < 8; ++pile) {
                for (int i = 0; i < pile; ++i) {
                    tableau[pile][i] = cards[c];
                    tableau[pile][i].face_up = (i == pile - 1);
                    ++c;
                }
            }

            // set stock
            std::copy(cards.begin() + c, cards.end(), stock.begin());

            // misc
            waste_cap = 0;
        }

        void move_cards(span<Card>& source, span<Card> dest) {
            for (int i = 0; i < source.size(); ++i) {
                dest[i] = source[i];
                source[i] = Card::NONE;
            }
        }

        void draw(uint8 n) {
            if (waste_cap == STOCK_SIZE) reset_stock();
            while (n > 0 and waste_cap < STOCK_SIZE) {
                ++waste_cap;
                if (stock[waste_cap - 1] != Card::NONE) {
                    --n;
                }
            }
        }

        void reset_stock() {
            waste_cap = 0;
        }

        void do_move(const Move m) {
            span<Card> source_span, dest_span;            
            switch (m.source) {
                case WASTE:
                    // if both source and dest are WASTE, move is draw
                    if (m.dest == WASTE) {
                        draw(m.source_offset);
                        return;
                    } else {
                        if (waste_cap == 0) throw std::out_of_range("Cannot move card from empty pile");
                        source_span = span<Card>(stock.begin() + waste_cap - 1)
                    }
            }
        } 
    }
}
