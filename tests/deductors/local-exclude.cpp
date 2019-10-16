#include <catch/catch.hpp>
#include "../../include/deductors/local-exclude.h"

using namespace AI;

TEST_CASE("LocalExcludeDeductor class", "[local-exclude-deductor]") {
    std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>> notes;
    Bot::SuggestionLog log;

    Bot::Player askPlayer = Bot::SCARLET;
    Bot::Player showPlayer = Bot::PLUM;

    LocalExcludeDeductor deductor(askPlayer);

    Bot::Player sugPlayer = Bot::SCARLET;
    Bot::Weapon sugWeapon = Bot::CANDLESTICK;
    Bot::Room sugRoom = Bot::BEDROOM;

    SECTION("no deductions") {
        REQUIRE_FALSE(notes[showPlayer][sugPlayer].has);
        REQUIRE_FALSE(notes[showPlayer][sugWeapon].has);
        REQUIRE_FALSE(notes[showPlayer][sugRoom].has);

        notes[showPlayer][sugPlayer].lacks = true;

        log.addSuggestion(askPlayer, Bot::Suggestion(sugPlayer, sugWeapon, sugRoom));
        log.addShow(showPlayer);

        REQUIRE_FALSE(deductor.run(log, notes));

        REQUIRE_FALSE(notes[showPlayer][sugPlayer].has);
        REQUIRE_FALSE(notes[showPlayer][sugWeapon].has);
        REQUIRE_FALSE(notes[showPlayer][sugRoom].has);
    }

    sugPlayer = Bot::Player(int(sugPlayer) + 1);
    sugWeapon = Bot::Weapon(int(sugWeapon) + 1);
    sugRoom = Bot::Room(int(sugRoom) + 1);

    SECTION("deduce room") {
        REQUIRE_FALSE(notes[showPlayer][sugPlayer].has);
        REQUIRE_FALSE(notes[showPlayer][sugWeapon].has);
        REQUIRE_FALSE(notes[showPlayer][sugRoom].has);

        notes[showPlayer][sugPlayer].lacks = true;
        notes[showPlayer][sugWeapon].lacks = true;

        log.addSuggestion(askPlayer, Bot::Suggestion(sugPlayer, sugWeapon, sugRoom));
        log.addShow(showPlayer);

        REQUIRE(deductor.run(log, notes));

        REQUIRE_FALSE(notes[showPlayer][sugPlayer].has);
        REQUIRE_FALSE(notes[showPlayer][sugWeapon].has);
        REQUIRE(notes[showPlayer][sugRoom].has);
        REQUIRE(notes[showPlayer][sugRoom].deduced);

        REQUIRE_FALSE(deductor.run(log, notes));
    }

    sugPlayer = Bot::Player(int(sugPlayer) + 1);
    sugWeapon = Bot::Weapon(int(sugWeapon) + 1);
    sugRoom = Bot::Room(int(sugRoom) + 1);

    SECTION("deduce weapon") {
        REQUIRE_FALSE(notes[showPlayer][sugPlayer].has);
        REQUIRE_FALSE(notes[showPlayer][sugWeapon].has);
        REQUIRE_FALSE(notes[showPlayer][sugRoom].has);

        notes[showPlayer][sugPlayer].lacks = true;
        notes[showPlayer][sugRoom].lacks = true;

        log.addSuggestion(askPlayer, Bot::Suggestion(sugPlayer, sugWeapon, sugRoom));
        log.addShow(showPlayer);

        REQUIRE(deductor.run(log, notes));

        REQUIRE_FALSE(notes[showPlayer][sugPlayer].has);
        REQUIRE(notes[showPlayer][sugWeapon].has);
        REQUIRE(notes[showPlayer][sugWeapon].deduced);
        REQUIRE_FALSE(notes[showPlayer][sugRoom].has);

        REQUIRE_FALSE(deductor.run(log, notes));
    }

    sugPlayer = Bot::Player(int(sugPlayer) + 1);
    sugWeapon = Bot::Weapon(int(sugWeapon) + 1);
    sugRoom = Bot::Room(int(sugRoom) + 1);

    SECTION("deduce player") {
        REQUIRE_FALSE(notes[showPlayer][sugPlayer].has);
        REQUIRE_FALSE(notes[showPlayer][sugWeapon].has);
        REQUIRE_FALSE(notes[showPlayer][sugRoom].has);

        notes[showPlayer][sugWeapon].lacks = true;
        notes[showPlayer][sugRoom].lacks = true;

        log.addSuggestion(askPlayer, Bot::Suggestion(sugPlayer, sugWeapon, sugRoom));
        log.addShow(showPlayer);

        REQUIRE(deductor.run(log, notes));

        REQUIRE(notes[showPlayer][sugPlayer].has);
        REQUIRE(notes[showPlayer][sugPlayer].deduced);
        REQUIRE_FALSE(notes[showPlayer][sugWeapon].has);
        REQUIRE_FALSE(notes[showPlayer][sugRoom].has);

        REQUIRE_FALSE(deductor.run(log, notes));
    }
}

// vim: set expandtab textwidth=100:
