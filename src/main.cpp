#include "game.h"
#include <iostream>

using std::cin, std::cout, std::endl;

int main(int argc, char** argv) {

    std::array<std::array<Card, TABLEAU_SIZE>, 7> tableau;
    std::array<std::array<Card, FOUNDATION_SIZE>, 4> foundation;
    std::array<Card, STOCK_SIZE> stock;

    for (array<Card, TABLEAU_SIZE>& t : tableau) t.fill(Card::NONE);
    for (array<Card, FOUNDATION_SIZE>& f : foundation) f.fill(Card::NONE);
    stock.fill(Card::NONE);
    
    tableau[0][0] = Card{10, SPADE, true};
    tableau[0][1] = Card{9, HEART, true};

    tableau[1][0] = Card{1, SPADE, false};
    tableau[1][1] = Card{8, SPADE, true};
    tableau[1][2] = Card{7, HEART, true};

    Klondike::State s(tableau, foundation, stock, 0);

    cout << s.debug_display();

    s.do_move(Move{TABLEAU, TABLEAU, 1, 1, 0, true});

    cout << s.debug_display();

    s.do_move(Move{TABLEAU, FOUNDATION, 1, 0, 0, false});

    cout << s.debug_display();
    
    return 0;
}
