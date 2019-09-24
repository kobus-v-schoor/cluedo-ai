#include <catch/catch.hpp>
#include "../../include/deductors/no-show.h"

using namespace AI;

TEST_CASE("NoShowDeductor", "[no-show-deductor]") {
    std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>> notes;
    Bot::SuggestionLog log;

    std::vector<Bot::Player> order = { Bot::SCARLET, Bot::PLUM, Bot::PEACOCK, Bot::GREEN };

    NoShowDeductor deductor(order);

    SECTION("no no-shows") {
        log.addSuggestion(Bot::SCARLET, Bot::Suggestion(Bot::SCARLET, Bot::CANDLESTICK, Bot::BEDROOM));
        log.addShow(Bot::PLUM);
        REQUIRE_FALSE(deductor.run(log, notes));
    }


    SECTION("single no-show") {
        REQUIRE_FALSE(notes[Bot::PLUM][Bot::SCARLET].lacks);
        REQUIRE_FALSE(notes[Bot::PLUM][Bot::CANDLESTICK].lacks);
        REQUIRE_FALSE(notes[Bot::PLUM][Bot::BEDROOM].lacks);

        log.addSuggestion(Bot::SCARLET, Bot::Suggestion(Bot::SCARLET, Bot::CANDLESTICK, Bot::BEDROOM));
        log.addShow(Bot::PEACOCK);
        REQUIRE(deductor.run(log, notes));

        REQUIRE(notes[Bot::PLUM][Bot::SCARLET].lacks);
        REQUIRE(notes[Bot::PLUM][Bot::CANDLESTICK].lacks);
        REQUIRE(notes[Bot::PLUM][Bot::BEDROOM].lacks);

        // check that it triggers only once
        REQUIRE_FALSE(deductor.run(log, notes));
    }

    SECTION("all no-shows") {
        for (auto p : { Bot::SCARLET, Bot::PLUM, Bot::PEACOCK, Bot::GREEN }) {
            REQUIRE_FALSE(notes[p][Bot::PLUM].lacks);
            REQUIRE_FALSE(notes[p][Bot::KNIFE].lacks);
            REQUIRE_FALSE(notes[p][Bot::BATHROOM].lacks);
        }

        log.addSuggestion(Bot::GREEN, Bot::Suggestion(Bot::PLUM, Bot::KNIFE, Bot::BATHROOM));
        log.addNoShow();

        REQUIRE(deductor.run(log, notes));

        for (auto p : { Bot::SCARLET, Bot::PLUM, Bot::PEACOCK }) {
            REQUIRE(notes[p][Bot::PLUM].lacks);
            REQUIRE(notes[p][Bot::KNIFE].lacks);
            REQUIRE(notes[p][Bot::BATHROOM].lacks);
        }

        // check that it doesn't make conclusions about the player who made the suggestion
        REQUIRE_FALSE(notes[Bot::GREEN][Bot::PLUM].lacks);
        REQUIRE_FALSE(notes[Bot::GREEN][Bot::KNIFE].lacks);
        REQUIRE_FALSE(notes[Bot::GREEN][Bot::BATHROOM].lacks);

        // check that it doesn't trigger twice
        REQUIRE_FALSE(deductor.run(log, notes));
    }
}

// vim: set expandtab textwidth=100:
