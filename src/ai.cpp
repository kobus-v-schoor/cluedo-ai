#include "../include/ai.h"
#include "../include/macros.h"

AI::Card::Card(Player p):
    card(int(p)),
    type(Card::Type::PLAYER)
{}

AI::Card::Card(Weapon w):
    card(int(w)),
    type(Card::Type::WEAPON)
{}

AI::Card::Card(Room r):
    card(int(r)),
    type(Card::Type::ROOM)
{}

NOT_IMPLEMENTED
AI::Position::Position(int pos){}

AI::Suggestion::Suggestion(Player p, Weapon w, Room r):
    player(p),
    weapon(w),
    room(r)
{}

NOT_IMPLEMENTED
AI::AI(const Player player){}

NOT_IMPLEMENTED
void AI::setCards(const std::vector<Card> cards){}

NOT_IMPLEMENTED
void AI::updateBoard(const std::vector<std::pair<Player, Position>> players){}

NOT_IMPLEMENTED
void AI::movePlayer(const Player player, Position position){}

NOT_IMPLEMENTED
void AI::madeSuggestion(Player player, AI::Suggestion suggestion){}

NOT_IMPLEMENTED
void AI::shownCard(Player asked, Player showed){}

NOT_IMPLEMENTED
AI::Position AI::getMove(int allowedMoves){return Position(0);}

NOT_IMPLEMENTED
AI::Suggestion AI::getSuggestion(){return Suggestion(Player(0),Weapon(0),Room(0));}

NOT_IMPLEMENTED
AI::Card AI::getCard(Suggestion suggestion){return Card(Player(0));}

// vim: set expandtab textwidth=100:
