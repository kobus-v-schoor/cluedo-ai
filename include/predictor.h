/**
 * \file predictor.h
 * \author Kobus van Schoor
 */

#pragma once

#include "bot.h"
#include "deck.h"
#include <map>

namespace AI {
    /**
     * This will serve as the base class for all the predictor classes. These classes will try and
     * identify high and low probability cards (i.e. cards with a high or low probability of being
     * in the envelope). This is done so that decisions made about which cards to suggest can be
     * made as efficiently as possible (by suggesting higher probability cards you are more likely
     * to suggest the correct cards earlier on in the game).
     */
    class Predictor {
        public:
            Predictor(){}
            virtual ~Predictor(){}

            /**
             * \brief Attempt to make a prediction about high-probability envelope cards
             */
            virtual void run(Deck& deck, std::map<Bot::Player,
                    std::map<Bot::Card, Bot::Notes>> notes, Bot::SuggestionLog log) =0;

        protected:
            bool contains(Deck& deck, Bot::Player player);
            bool contains(Deck& deck, Bot::Weapon weapon);
            bool contains(Deck& deck, Bot::Room room);
    };
}

// vim: set expandtab textwidth=100:
