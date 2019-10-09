/**
 * \file multiple.h
 * \author Kobus van Schoor
 */

#pragma once
#include "../predictor.h"

namespace AI {
    /**
     * If a player has made a suggestion multiple times for a card that they don't have, there is a
     * chance that it is an envelope card
     */
    class MultiplePredictor : public Predictor {
        public:
            MultiplePredictor(){}

            void run(Deck& deck, std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>> notes,
                    Bot::SuggestionLog log) override;
    };
}

// vim: set expandtab textwidth=100:
