/**
 * \file deck.h
 * \author Kobus van Schoor
 */

#pragma once

#include "bot.h"
#include <map>
#include <vector>

namespace AI {

    /**
     * \brief Used to group multiple cards and their probability scores
     *
     * This will be used to group multiple cards, while keeping the card types separate. The
     * struct will also contain the probability scores for the various cards;
     */
    struct Deck {
        std::vector<Bot::Player> players;
        std::vector<Bot::Weapon> weapons;
        std::vector<Bot::Room> rooms;

        std::map<Bot::Card, int> scores;
    };
};

// vim: set expandtab textwidth=100:
