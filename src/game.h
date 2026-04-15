#pragma once

#include "util.h"
#include <array>
#include <span>
#include <stdexcept>
#include <cassert>
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>

using std::array, std::span, std::string, std::stringstream;
using std::cout;

const size_t TABLEAU_SIZE = 19;
const size_t FOUNDATION_SIZE = 13;
const size_t STOCK_SIZE = 24;

struct Klondike {
    struct State {
        array<array<Card, TABLEAU_SIZE>, 7> tableau;
        array<array<Card, FOUNDATION_SIZE>, 4> foundation;
        array<Card, STOCK_SIZE> stock;
        uint waste_cap;

        State(const Deck& cards) {
            // init arrays with NONE cards
            for (array<Card, TABLEAU_SIZE>& t : tableau) t.fill(Card::NONE);
            for (array<Card, FOUNDATION_SIZE>& f : foundation) f.fill(Card::NONE);

            int c = 0;  // card counter

            // deal tableau
            for (int pile = 0; pile < 7; ++pile) {
                for (int i = 0; i < pile + 1; ++i) {
                    tableau[pile][i] = cards[c];
                    tableau[pile][i].face_up = (i == pile);
                    ++c;
                }
            }

            // set stock
            std::copy(cards.begin() + c, cards.end(), stock.begin());

            // misc
            waste_cap = 0;
        }

        void move_cards(span<Card>& source, span<Card> dest) {
            assert(source.size() == dest.size());  // should be guaranteed by do_move
            for (int i = 0; i < source.size(); ++i) {
                dest[i] = source[i];
                source[i] = Card::NONE;
            }
        }

        void draw(uint8 n) {
            if (waste_cap == STOCK_SIZE) reset_stock();
            while (n > 0 and waste_cap < STOCK_SIZE) {
                ++waste_cap;
                stock[waste_cap - 1].face_up = true;
                if (stock[waste_cap - 1] != Card::NONE) {
                    --n;
                }
            }
        }

        void reset_stock() {
            waste_cap = 0;
            for (Card& c : stock) c.face_up = false;
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
                        source_span = span<Card>(stock.begin() + waste_cap - 1, stock.begin() + waste_cap);
                    }
                    break;
                case FOUNDATION:
                    if (foundation[m.source_pile][m.source_offset] == Card::NONE) throw std::out_of_range("Cannot move empty stack of cards");
                    assert(m.dest == TABLEAU);  // should only be possible to move from foundation to tableau
                    source_span = span<Card>(foundation[m.source_pile].begin() + m.source_offset, std::find(foundation[m.source_pile].begin() + m.source_offset, foundation[m.source_pile].end(), Card::NONE));
                    break;
                case TABLEAU:
                    if (tableau[m.source_pile][m.source_offset] == Card::NONE) throw std::out_of_range("Cannot move empty stack of cards");
                    source_span = span<Card>(tableau[m.source_pile].begin() + m.source_offset, std::find(tableau[m.source_pile].begin() + m.source_offset, tableau[m.source_pile].end(), Card::NONE));
                    break;                   
            }
            switch (m.dest) {
                case WASTE:
                    throw std::invalid_argument("Cannot move cards to waste pile");
                    break;
                case FOUNDATION:
                    dest_span = span<Card>(foundation[m.dest_pile].begin(), foundation[m.dest_pile].begin() + source_span.size());
                    break;
                case TABLEAU:
                    dest_span = span<Card>(tableau[m.dest_pile].begin(), tableau[m.dest_pile].begin() + source_span.size());
                    break;
            }
            move_cards(source_span, dest_span);
        }

        string debug_display() const {
            stringstream out;

            // foundation
            out << "foundation:\n";
            for (const array<Card, FOUNDATION_SIZE>& f : foundation) {
                for (const Card& c : f) {
                    out << c.debug_display() << " ";
                }
                out << "\n";
            }
            out << "\n";

            // stock
            out << "stock:\n";
            int counter = 0;
            for (const Card& c : stock) {
                if (counter == waste_cap) out << "| ";
                out << c.debug_display() << " ";
                ++counter;
            }
            out << "\n\n";

            // tableau
            out << "tableau:\n";
            for (const array<Card, TABLEAU_SIZE>& t : tableau) {
                for (const Card& c : t) {
                    out << c.debug_display() << " ";
                }
                out << "\n";
            }
            out << "\n";

            return out.str();
        }
    };
};
