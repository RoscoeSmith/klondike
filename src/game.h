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
#include <vector>

using std::array, std::span, std::vector, std::string, std::stringstream;
using std::cout;

const size_t TABLEAU_SIZE = 19;
const size_t FOUNDATION_SIZE = 13;
const size_t STOCK_SIZE = 24;

struct Klondike {
    struct State {
        array<array<Card, TABLEAU_SIZE>, 7> tableau;
        array<array<Card, FOUNDATION_SIZE>, 4> foundation;
        array<Card, STOCK_SIZE> stock;
        uint8 waste_cap;

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

        State(const array<array<Card, TABLEAU_SIZE>, 7> given_tableau, const array<array<Card,FOUNDATION_SIZE>, 4> given_foundation, const array<Card, STOCK_SIZE> given_stock, uint8 given_waste_cap)
            : tableau(given_tableau), foundation(given_foundation), stock(given_stock), waste_cap(given_waste_cap) {}

        void move_cards(span<Card>& source, span<Card>& dest) {
            assert(source.size() == dest.size());  // should be guaranteed by do_move

            cout << "source size: " << source.size() << ", dest size: " << dest.size() << "\n";
            cout << "view into source:";
            for (const Card& c : source) cout << c;
            cout << "\nview into dest:";
            for (const Card& c : dest) cout << c;
            cout << "\n";
            
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
                if (!stock[waste_cap - 1].is_none()) {
                    --n;
                }
            }
        }

        void undo_draw(uint8 n, bool unrecycle) {
            while (n > 0 and waste_cap > 0) {
                if (!stock[waste_cap - 1].is_none()) {
                    --n;
                }
                --waste_cap;
                stock[waste_cap].face_up = false;
            }
            if (unrecycle and n == 0) {
                waste_cap = STOCK_SIZE;
                for (Card& c : stock) c.face_up = true;
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
                    if (m.source_offset > 0 and not tableau[m.source_pile][m.source_offset - 1].face_up) {
                        tableau[m.source_pile][m.source_offset - 1].face_up = true;
                    }
                    break;                   
            }
            switch (m.dest) {
                case WASTE:
                    throw std::invalid_argument("Cannot move cards to waste pile");
                    break;
                case FOUNDATION:
                    dest_span = span<Card>(foundation[m.dest_pile].begin() + m.dest_offset, foundation[m.dest_pile].begin() + m.dest_offset + source_span.size());
                    break;
                case TABLEAU:
                    dest_span = span<Card>(tableau[m.dest_pile].begin() + m.dest_offset, tableau[m.dest_pile].begin() + m.dest_offset + source_span.size());
                    break;
            }
            move_cards(source_span, dest_span);
        }

        void undo_move(const Move m) {
            span<Card> source_span, dest_span;
            switch (m.source) {
                case WASTE:
                    // if both source and dest are WASTE, move is draw
                    if (m.dest == WASTE) {
                        undo_draw(m.source_offset, m.extra);
                        return;
                    } else {
                        assert(waste_cap > 0 and stock[waste_cap - 1].is_none());
                        dest_span = span<Card>(stock.begin() + waste_cap - 1, stock.begin() + waste_cap);
                    }
                    break;
                case FOUNDATION:
                    break;
                case TABLEAU:
                    break;
            }
            switch (m.dest) {
                case WASTE:
                    throw std::invalid_argument("Cannot move cards to waste pile (since this was triggered in undo_move, move was ill-formatted)");
                    break;
                case FOUNDATION:
                    break;
                case TABLEAU:
                    break;
            }
        }

        const int last_card_idx(const span<const Card> pile) const {
            return std::distance(pile.begin(), std::find(pile.begin(), pile.end(), Card::NONE)) - 1;
        }

        // get index of last non-NONE card in tableau piles
        // (add one to each value to get first NONE card)
        const array<int, 7> tableau_last_card_idxs() const {
            array<int, 7> idxs;
            for (int i = 0; i < 7; ++i) {
                idxs[i] = last_card_idx(span<const Card>(tableau[i].begin(), tableau[i].end()));
            }
            return idxs;
        }

        // get index of last non-NONE card in foundation piles
        // (add one to each value to get first NONE card)
        const array<int, 4> foundation_last_card_idxs() const {
            array<int, 4> idxs;
            for (int i = 0; i < 4; ++i) {
                idxs[i] = last_card_idx(span<const Card>(foundation[i].begin(), foundation[i].end()));
            }
            return idxs;
        }

        const int first_empty_card_idx(const span<const Card> pile) const {
            int i = last_card_idx(pile) + 1;
            if (i == pile.size()) i = -1;
            return i;
        }

        const array<int, 7> tableau_first_empty_card_idxs() const {
            array<int, 7> idxs = tableau_last_card_idxs();
            for (int i = 0; i < 7; ++i) {
                ++idxs[i];
                if (idxs[i] == TABLEAU_SIZE) idxs[i] = -1;
            }
            return idxs;
        }

        const array<int, 4> foundation_first_empty_card_idxs() const {
            array<int, 4> idxs = foundation_last_card_idxs();
            for (int i = 0; i < 4; ++i) {
                ++idxs[i];
                if (idxs[i] == FOUNDATION_SIZE) idxs[i] = -1;
            }
            return idxs;
        }

        const int first_face_up_card_idx(const span<const Card> pile) const {
            int i = std::distance(pile.begin(), std::find_if(pile.begin(), pile.end(), [](const Card& c) { return c.face_up; }));
            if (i == pile.size()) return -1;
            else return i;
        }

        const array<int, 7> tableau_first_face_up_card_idxs() const {
            array<int, 7> idxs;
            for (int i = 0; i < 7; ++i) {
                idxs[i] = first_face_up_card_idx(span<const Card>(tableau[i].begin(), tableau[i].end()));
            }
            return idxs;
        }

        const array<int, 4> foundation_first_face_up_card_idxs() const {
            array<int, 4> idxs;
            for (int i = 0; i < 4; ++i) {
                idxs[i] = first_face_up_card_idx(span<const Card>(foundation[i].begin(), foundation[i].end()));
            }
            return idxs;
        }

        // get amount of non-NONE cards left in stock
        const uint8 stock_left() const {
            return STOCK_SIZE - waste_cap - std::count(stock.begin() + waste_cap, stock.end(), Card::NONE);
            
        }

        const vector<Move> get_legal_moves(const uint8 draw_amount) const {
            vector<Move> legal_moves;

            // get all last cards in piles
            const array<int, 7> tableau_last_idxs = tableau_last_card_idxs();
            const array<int, 4> foundation_last_idxs = foundation_last_card_idxs();

            // get all face-up cards moveable to each last card
            const array<int, 7> tableau_first_idxs = tableau_first_face_up_card_idxs();
            const array<int, 4> foundation_first_idxs = foundation_first_face_up_card_idxs();

            // moves to tableau
            for (int dest = 0; dest < 7; ++dest) {
                if (tableau_last_idxs[dest] == TABLEAU_SIZE) continue;  // impossible to move onto
                if (tableau_last_idxs[dest] == -1) {
                    // pile is empty, look for kings
                    assert(tableau_first_idxs[dest] == -1);

                    // check other tableau piles
                    for (int source = 0; source < 7; ++source) {
                        if (source == dest) continue;
                        if (tableau[source][tableau_first_idxs[source]] >> Card::NONE) {
                            bool reveal = tableau_first_idxs[source] > 0 and !tableau[source][tableau_first_idxs[source - 1]].face_up;
                            legal_moves.emplace_back(TABLEAU, TABLEAU, static_cast<uint8>(source), static_cast<uint8>(tableau_first_idxs[source]), static_cast<uint8>(dest), 0, reveal);
                        }
                    }

                    // check waste
                    if (waste_cap > 0 and stock[waste_cap - 1] >> Card::NONE) {
                        legal_moves.emplace_back(WASTE, TABLEAU, 0, waste_cap - 1, dest, 0, false);
                    }
                } else {
                    // look for card that can go next in pile
                    const Card last_card = tableau[dest][tableau_last_idxs[dest]];
                    assert(last_card.face_up);

                    // check other tableau piles
                    for (int source = 0; source < 7; ++source) {
                        if (source == dest) continue;
                        const Card first_card = tableau[source][tableau_first_idxs[source]];
                        int walk = first_card.rank - (last_card.rank - 1);
                        if (tableau_first_idxs[source] + walk > tableau_last_idxs[source]) continue;  // sequence ends before possible valid card
                        // could do sequence parity check but is it worth it?
                        if (tableau[source][tableau_first_idxs[source] + walk] >> last_card) {
                            // legal move
                            bool reveal = tableau_first_idxs[source] > 0 and !tableau[source][tableau_first_idxs[source - 1]].face_up;
                            legal_moves.emplace_back(TABLEAU, TABLEAU, static_cast<uint8>(source), static_cast<uint8>(tableau_first_idxs[source] + walk), static_cast<uint8>(dest), static_cast<uint8>(tableau_last_idxs[dest] + 1), reveal);
                        }
                    }

                    // check waste
                    if (waste_cap > 0 and stock[waste_cap - 1] >> last_card) {
                        legal_moves.emplace_back(WASTE, TABLEAU, 0, waste_cap - 1, dest, 0, false);
                    }
                }
            }

            // moves to foundation
            // TODO: implement

            // draw move
            legal_moves.push_back(Move::draw(std::min(draw_amount, stock_left())));

            return legal_moves;
        }

        string debug_display() const {
            stringstream out;

            // foundation
            out << "foundation:\n";
            for (const array<Card, FOUNDATION_SIZE>& f : foundation) {
                for (const Card& c : f) {
                    out << c;
                }
                out << "\n";
            }
            out << "\n";

            // stock
            out << "stock:\n";
            int counter = 0;
            for (const Card& c : stock) {
                if (counter == waste_cap) out << "| ";
                out << c;
                ++counter;
            }
            out << "\n\n";

            // tableau
            out << "tableau:\n";
            for (const array<Card, TABLEAU_SIZE>& t : tableau) {
                for (const Card& c : t) {
                    out << c;
                }
                out << "\n";
            }
            out << "\n";

            return out.str();
        }
    };
};

struct RichMove : public Move {
    // TODO: add card data from given state to given move, return as rich move
};
