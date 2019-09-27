/**
 * \file predictor.h
 * \author Kobus van Schoor
 */

#pragma once

#include<vector>
#include<map>

namespace AI {
    class Predictor {
        public:
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

            Predictor(){}
            virtual ~Predictor(){}

            void run(Deck& deck);
    };
}

// vim: set expandtab textwidth=100:
