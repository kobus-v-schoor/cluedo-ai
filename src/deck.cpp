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

// vim: set expandtab textwidth=100:
