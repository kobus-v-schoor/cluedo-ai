/**
 * \file predictor.cpp
 * \author Kobus van Schoor
 */

#include "../include/predictor.h"
#include <algorithm>

using namespace AI;

bool Predictor::contains(Deck& deck, Bot::Player player)
{
    return std::find(deck.players.begin(), deck.players.end(), player) != deck.players.end();
}

bool Predictor::contains(Deck& deck, Bot::Weapon weapon)
{
    return std::find(deck.weapons.begin(), deck.weapons.end(), weapon) != deck.weapons.end();
}

bool Predictor::contains(Deck& deck, Bot::Room room)
{
    return std::find(deck.rooms.begin(), deck.rooms.end(), room) != deck.rooms.end();
}

// vim: set expandtab textwidth=100:
