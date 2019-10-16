/**
 * \file no-show.h
 * \author Kobus van Schoor
 */

#pragma once
#include "../predictor.h"

namespace AI {
    /**
     * If a player makes a suggestion, has 2 cards in the suggestion and nobody was able to show a
     * card, there is a very high probability that the 3 card is an envelope card
     */
    class NoShowPredictor : public Predictor {
        public:
            NoShowPredictor(Bot::Player player);

            void run(Deck& deck, std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>> notes,
                    Bot::SuggestionLog log) override;
    };
}

// vim: set expandtab textwidth=100:
