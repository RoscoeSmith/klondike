#include "game.h"
#include <iostream>

using std::cin, std::cout, std::endl;

int main(int argc, char** argv) {

    std::array<std::array<Card, TABLEAU_SIZE>, 7> tableau;
    std::array<std::array<Card, FOUNDATION_SIZE>, 4> foundation;
    std::array<Card, STOCK_SIZE> stock;

    for (array<Card, TABLEAU_SIZE>& t : tableau) t.fill(Card::NONE);
    for (array<Card, FOUNDATION_SIZE>& f : foundation) f.fill(Card::NONE);
    // stock.fill(Card::NONE);
    stock.fill(Card{7, CLUB, false});
    
    tableau[0][0] = Card{10, SPADE, true};
    tableau[0][1] = Card{9, HEART, true};

    tableau[1][0] = Card{1, CLUB, false};
    tableau[1][1] = Card{8, SPADE, true};
    tableau[1][2] = Card{7, HEART, true};

    stock[0] = Card{4, SPADE, false};
    stock[1] = Card{6, DIAMOND, false};
    stock[2] = Card{2, CLUB, false};
    stock[3] = Card{9, HEART, false};
    stock[4] = Card{11, DIAMOND, false};
    stock[5] = Card{6, CLUB, false};
    stock[6] = Card{1, SPADE, false};
    stock[7] = Card{13, DIAMOND, false};
    stock[8] = Card{13, HEART, false};

    stock[23] = Card{1, DIAMOND, false};

    Klondike::State s(tableau, foundation, stock, 0);

    std::vector<Move> moves;

    cout << s.debug_display();

    cout << "legal moves:\n";
    moves = s.get_legal_moves(3);
    for (const auto& m : moves) {
        RichMove rm = RichMove::make(m, s);
        cout << "\t" << rm << "\n";
    }

    s.do_move(Move{TABLEAU, TABLEAU, 1, 1, 0, 2, true});

    cout << s.debug_display();

    cout << "legal moves:\n";
    moves = s.get_legal_moves(3);
    for (const auto& m : moves) {
        RichMove rm = RichMove::make(m, s);
        cout << "\t" << rm << "\n";
    }

    s.do_move(Move{TABLEAU, FOUNDATION, 1, 0, 0, 0, false});

    cout << s.debug_display();

    cout << "legal moves:\n";
    moves = s.get_legal_moves(3);
    for (const auto& m : moves) {
        RichMove rm = RichMove::make(m, s);
        cout << "\t" << rm << "\n";
    }

    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    s.do_move(Move{WASTE, TABLEAU, 0, 0, 0, 4, false});

    cout << s.debug_display();

    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    // should be the last draw?
    s.do_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});
    
    cout << s.debug_display();

    cout << "===  UNDO  ===\n";

    s.undo_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});

    cout << s.debug_display();

    cout << "===  UNDO  ===\n";

    s.undo_move(Move{WASTE, WASTE, 0, 3, 0, 0, false});

    cout << s.debug_display();

    return 0;
}
