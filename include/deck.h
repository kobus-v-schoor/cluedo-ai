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
     * struct will also contain the probability scores for the various cards. A higher score means
     * that the card is more likely to _not_ be in the envelope.
     */
    struct Deck {
        std::vector<Bot::Player> players;
        std::vector<Bot::Weapon> weapons;
        std::vector<Bot::Room> rooms;

        std::map<Bot::Card, int> scores;

        /**
         * \brief sorts the cards by their probability scores from highest probability (lower score)
         * to lower probability (higher score)
         */
        void sort();

        /**
         * \brief returns true if the given card is in one of the decks
         */
        bool contains(Bot::Card card);
    };
};

// vim: set expandtab textwidth=100:
