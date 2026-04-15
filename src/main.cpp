#include "game.h"
#include <iostream>

using std::cin, std::cout, std::endl;

int main(int argc, char** argv) {
    const Deck deck = shuffled_deck();

    cout << "unshuffled deck:\n";
    for (const Card& c : UNSHUFFLED_DECK) cout << c.debug_display() << " ";
    cout << "\n\n";

    cout << "deck:\n";
    for (const Card& c : deck) cout << c.debug_display() << " ";
    cout << "\n\n";
    
    Klondike::State s(deck);

    cout << s.debug_display();

    s.draw(3);

    cout << s.debug_display();

    s.do_move(Move{WASTE, FOUNDATION, 0, 0, 0, false});

    cout << s.debug_display();

    s.draw(STOCK_SIZE - 3);

    cout << s.debug_display();

    s.draw(3);

    cout << s.debug_display();
    
    return 0;
}
