/**
 * \file no-show.h
 * \author Kobus van Schoor
 */

#pragma once
#include "../deductor.h"

namespace AI {
    /**
     * \brief Marks that a player doesn't have any of the three cards if they are unable to show a
     * card after a suggestion.
     *
     * If a player is unable to show a card after a suggestion was made it means that they cannot
     * have any of the three cards in the suggestion. This means that the three cards in the
     * suggestion can be marked as "lacks" for the player that couldn't show anything
     */
    class NoShowDeductor : public Deductor {
        public:
            NoShowDeductor(std::vector<Bot::Player> order);

            bool run(Bot::SuggestionLog log, std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>>& notes) override;

        private:
            std::vector<Bot::Player> order;
    };
}

// vim: set expandtab textwidth=100:
