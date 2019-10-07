/**
 * \file card-count-exclude.h
 * \author Kobus van Schoor
 */

#pragma once
#include "../deductor.h"

namespace AI {
    /**
     * \brief By calculating the amount of cards in the players hand we can mark cards as lacking
     * once we know what cards they have
     *
     * Once we calculate how many cards every player has, we can mark all other cards lacking once
     * we have have marked the player has the amount of cards dealt to them
     */
    class CardCountExclude : public Deductor {
        public:
            CardCountExclude(){}

            bool run(Bot::SuggestionLog log, std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>>& notes) override;
        private:
            /**
             * If true, it will use the amount of players in the order to calculate the amount of
             * cards dealt to every player. If false, it will use the notes to determine the cards
             * dealt to this player and assume everybody has the same amount of cards as us
             */
            static const bool useOrderForCount = true;
    };
}

// vim: set expandtab textwidth=100:
