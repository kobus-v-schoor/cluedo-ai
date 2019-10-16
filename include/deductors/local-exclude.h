/**
 * \file local-exclude.h
 * \author Kobus van Schoor
 */

#pragma once
#include "../deductor.h"

namespace AI {
    /**
     * \brief Detects if a player has shown a card but we know they don't have two of the three
     * cards in the suggestion
     *
     * This will detect when a player showed a card because of a suggestion, but we know they don't
     * have two of the three cards, hence they have the third card. Example:
     *
     * Suggestion: Mustard, Rope, Kitchen
     * Player showes something
     * We know they don't have Rope or Kitchen
     * Hence they must have Mustard
     *
     */
    class LocalExcludeDeductor : public Deductor {
        public:
            LocalExcludeDeductor(Bot::Player player);

            bool run(Bot::SuggestionLog log, std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>>& notes) override;
    };
}

// vim: set expandtab textwidth=100:
