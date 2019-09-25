#include <catch/catch.hpp>
#include "../../include/deductors/local-exclude.h"

using namespace AI;

TEST_CASE("LocalExcludeDeductor class", "[local-exclude-deductor]") {
    std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>> notes;
    Bot::SuggestionLog log;

    LocalExcludeDeductor deductor;

    SECTION("deduce room") {
        REQUIRE_FALSE(notes[Bot::WHITE][Bot::BEDROOM].has);
        REQUIRE_FALSE(notes[Bot::WHITE][Bot::BEDROOM].deduced);

        notes[Bot::WHITE][Bot::SCARLET].lacks = true;
        notes[Bot::WHITE][Bot::CANDLESTICK].lacks = true;

        log.addSuggestion(Bot::SCARLET, Bot::Suggestion(Bot::SCARLET, Bot::CANDLESTICK, Bot::BEDROOM));
        log.addShow(Bot::WHITE);

        // check if deduction was made
        REQUIRE(deductor.run(log, notes));
        // check if correct flags has been set
        REQUIRE(notes[Bot::WHITE][Bot::BEDROOM].has);
        REQUIRE(notes[Bot::WHITE][Bot::BEDROOM].deduced);
        // check that deduction wasn't made for a second time
        REQUIRE_FALSE(deductor.run(log, notes));
    }

    SECTION("deduce weapon") {
        REQUIRE_FALSE(notes[Bot::WHITE][Bot::KNIFE].has);
        REQUIRE_FALSE(notes[Bot::WHITE][Bot::KNIFE].deduced);

        notes[Bot::WHITE][Bot::PLUM].lacks = true;
        notes[Bot::WHITE][Bot::BATHROOM].lacks = true;

        log.addSuggestion(Bot::SCARLET, Bot::Suggestion(Bot::PLUM, Bot::KNIFE, Bot::BATHROOM));
        log.addShow(Bot::WHITE);

        // check if deduction was made
        REQUIRE(deductor.run(log, notes));
        // check if correct flags has been set
        REQUIRE(notes[Bot::WHITE][Bot::KNIFE].has);
        REQUIRE(notes[Bot::WHITE][Bot::KNIFE].deduced);
        // check that deduction wasn't made for a second time
        REQUIRE_FALSE(deductor.run(log, notes));
    }

    SECTION("deduce player") {
        REQUIRE_FALSE(notes[Bot::WHITE][Bot::PEACOCK].has);
        REQUIRE_FALSE(notes[Bot::WHITE][Bot::PEACOCK].deduced);

        notes[Bot::WHITE][Bot::LEAD_PIPE].lacks = true;
        notes[Bot::WHITE][Bot::STUDY].lacks = true;

        log.addSuggestion(Bot::SCARLET, Bot::Suggestion(Bot::PEACOCK, Bot::LEAD_PIPE, Bot::STUDY));
        log.addShow(Bot::WHITE);

        // check if deduction was made
        REQUIRE(deductor.run(log, notes));
        // check if correct flags has been set
        REQUIRE(notes[Bot::WHITE][Bot::PEACOCK].has);
        REQUIRE(notes[Bot::WHITE][Bot::PEACOCK].deduced);
        // check that deduction wasn't made for a second time
        REQUIRE_FALSE(deductor.run(log, notes));
    }
}

// vim: set expandtab textwidth=100:
