/**
 * \file predictor.h
 * \author Kobus van Schoor
 */

#pragma once

#include "bot.h"
#include "deck.h"
#include <map>

namespace AI {
    class Predictor {
        public:
            Predictor(){}
            virtual ~Predictor(){}

            /**
             * \brief Attempt to make a prediction about high-probability envelope cards
             */
            virtual void run(Deck& deck, std::map<Bot::Player,
                    std::map<Bot::Card, Bot::Notes>> notes, Bot::SuggestionLog log) =0;
    };
}

// vim: set expandtab textwidth=100:
