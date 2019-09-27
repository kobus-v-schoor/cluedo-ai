/**
 * \file seen.h
 * \author Kobus van Schoor
 */

#pragma once
#include "../predictor.h"

namespace AI {
    /**
     * If a player is shown a card from a suggestion, and we know that they have already seen two
     * cards from that suggestion, there is a good chance that the person showed them the third
     * card. This means we should avoid the third card since the person who showed the card probably
     * has it
     */
    class SeenPredictor : public Predictor {
        public:
            SeenPredictor(){}

            void run(Deck& deck, std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>> notes,
                    Bot::SuggestionLog log) override;
    };
}

// vim: set expandtab textwidth=100:
