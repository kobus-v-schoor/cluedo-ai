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
    class CardCountExcludeDeductor : public Deductor {
        public:
            CardCountExcludeDeductor(Bot::Player player, std::vector<Bot::Player> order);

            bool run(Bot::SuggestionLog log, std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>>& notes) override;
        private:
            std::vector<Bot::Player> order;
            static std::vector<Bot::Card> allCards;
    };
}

// vim: set expandtab textwidth=100:
