#include "../include/ai.h"

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

AI::Position::Position(int pos){}

AI::Suggestion::Suggestion(Player p, Weapon w, Room r):
    player(p),
    weapon(w),
    room(r)
{}

AI::AI(const Player player){}

void AI::setCards(const std::vector<Card> cards){}

void AI::updateBoard(const std::vector<std::pair<Player, Position>> players){}

void AI::movePlayer(const Player player, Position position){}

void AI::madeSuggestion(Player player, AI::Suggestion suggestion){}

void AI::shownCard(Player asked, Player showed){}

AI::Position AI::getMove(int allowedMoves){return Position(0);}

AI::Suggestion AI::getSuggestion(){return Suggestion(Player(0),Weapon(0),Room(0));}

AI::Card AI::getCard(Suggestion suggestion){return Card(Player(0));}

// vim: set expandtab textwidth=100:
