/**
 * \file bot.cpp
 * \author Kobus van Schoor
 */

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

bool Bot::Card::operator<(const Card& other) const
{
    return type == other.type ? card < other.card : type < other.type;
}

bool Bot::Card::operator==(const Card& other) const
{
    return (type == other.type) && (card == other.card);
}

Bot::Suggestion::Suggestion(Player p, Weapon w, Room r):
    player(p),
    weapon(w),
    room(r)
{}

Bot::Bot(const Player player) :
    player(player)
{}

void Bot::setCards(const std::vector<Card> cards)
{
    for (auto c : cards)
        marksheet[player][c].seen = true;
}

void Bot::updateBoard(const std::vector<std::pair<Player, Position>> players)
{
    board.clear();
    for (auto p : players)
        board[p.first] = p.second;
}

void Bot::movePlayer(const Player player, Position position)
{
    board[player] = position;
}

void Bot::madeSuggestion(Player player, Suggestion suggestion){}

void Bot::shownCard(Player asked, Player showed){}

int Bot::getMove(int allowedMoves){ return 0; }

Bot::Suggestion Bot::getSuggestion(){ return Suggestion(Player(0),Weapon(0),Room(0)); }

void Bot::showCard(Player player, Card card)
{
    marksheet[this->player][card].seen = true;
    marksheet[player][card].shown = true;
}

void Bot::noShowCard(){}

Bot::Card Bot::getCard(std::vector<Card> cards){ return Card(Player(0)); }

// vim: set expandtab textwidth=100:
