/**
 * \file seen.h
 * \author Kobus van Schoor
 */

#pragma once
#include "../deductor.h"

namespace AI {
    /**
     * \brief Marks when another player has seen, and thus knows about, another card.
     *
     * By knowing what card has been shown (by elimination), we can deduce what cards another player
     * has seen. By knowing what the other player knows we could possibly use this to our advantage.
     */
    class SeenDeductor : public Deductor {
        public:
            SeenDeductor(Bot::Player player);

            bool run(Bot::SuggestionLog log, std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>>& notes) override;
    };
}

// vim: set expandtab textwidth=100:
