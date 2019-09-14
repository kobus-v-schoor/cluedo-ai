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

NOT_IMPLEMENTED
AI::AI(Player player){}

NOT_IMPLEMENTED
void AI::setCards(std::vector<Card> cards){}

NOT_IMPLEMENTED
void AI::updateBoard(std::vector<std::pair<Player, Position>> players){}
