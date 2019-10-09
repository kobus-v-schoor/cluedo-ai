/**
 * \file deck.cpp
 * \author Kobus van Schoor
 */

#include "../include/deck.h"
#include <algorithm>

using namespace AI;

void Deck::sort()
{
    std::sort(players.begin(), players.end(), [&](Bot::Player a, Bot::Player b)
            { return scores[a] < scores[b]; });
    std::sort(weapons.begin(), weapons.end(), [&](Bot::Weapon a, Bot::Weapon b)
            { return scores[a] < scores[b]; });
    std::sort(rooms.begin(), rooms.end(), [&](Bot::Room a, Bot::Room b)
            { return scores[a] < scores[b]; });
}

bool Deck::contains(Bot::Card c)
{
    switch(c.type) {
        case Bot::Card::PLAYER: return std::find(players.begin(), players.end(), Bot::Player(c.card)) != players.end();
        case Bot::Card::WEAPON: return std::find(weapons.begin(), weapons.end(), Bot::Weapon(c.card)) != weapons.end();
        case Bot::Card::ROOM: return std::find(rooms.begin(), rooms.end(), Bot::Room(c.card)) != rooms.end();
    }

    return false;
}

// vim: set expandtab textwidth=100:
