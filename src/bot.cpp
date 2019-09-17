#include "../include/bot.h"

using namespace AI;

Bot::Card::Card(Bot::Player p):
    card(int(p)),
    type(Card::Type::PLAYER)
{}

Bot::Card::Card(Bot::Weapon w):
    card(int(w)),
    type(Card::Type::WEAPON)
{}

Bot::Card::Card(Bot::Room r):
    card(int(r)),
    type(Card::Type::ROOM)
{}

Bot::Suggestion::Suggestion(Player p, Weapon w, Room r):
    player(p),
    weapon(w),
    room(r)
{}

Bot::Bot(const Player player){}

void Bot::setCards(const std::vector<Card> cards){}

void Bot::updateBoard(const std::vector<std::pair<Player, Position>> players){}

void Bot::movePlayer(const Player player, Position position){}

void Bot::madeSuggestion(Player player, Suggestion suggestion){}

void Bot::shownCard(Player asked, Player showed){}

int Bot::getMove(int allowedMoves){ return 0; }

Bot::Suggestion Bot::getSuggestion(){ return Suggestion(Player(0),Weapon(0),Room(0)); }

Bot::Card Bot::getCard(std::vector<Card> cards){ return Card(Player(0)); }

// vim: set expandtab textwidth=100:
