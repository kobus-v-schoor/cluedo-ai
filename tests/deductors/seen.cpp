#include <catch/catch.hpp>
#include "../../include/deductors/seen.h"

using namespace AI;

TEST_CASE("SeenDeductor", "[seen-deductor]") {
    std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>> notes;
    Bot::SuggestionLog log;

    Bot::Player askPlayer = Bot::SCARLET;
    Bot::Player showPlayer = Bot::PLUM;

    SeenDeductor deductor(askPlayer);

    Bot::Player sugPlayer = Bot::SCARLET;
    Bot::Weapon sugWeapon = Bot::CANDLESTICK;
    Bot::Room sugRoom = Bot::BEDROOM;

    SECTION("no deductions") {
        REQUIRE_FALSE(notes[askPlayer][sugPlayer].seen);
        REQUIRE_FALSE(notes[askPlayer][sugWeapon].seen);
        REQUIRE_FALSE(notes[askPlayer][sugRoom].seen);

        notes[showPlayer][sugPlayer].lacks = true;

        log.addSuggestion(askPlayer, Bot::Suggestion(sugPlayer, sugWeapon, sugRoom));
        log.addShow(showPlayer);

        REQUIRE_FALSE(deductor.run(log, notes));

        REQUIRE_FALSE(notes[askPlayer][sugPlayer].seen);
        REQUIRE_FALSE(notes[askPlayer][sugWeapon].seen);
        REQUIRE_FALSE(notes[askPlayer][sugRoom].seen);
    }

    sugPlayer = Bot::Player(int(sugPlayer) + 1);
    sugWeapon = Bot::Weapon(int(sugWeapon) + 1);
    sugRoom = Bot::Room(int(sugRoom) + 1);

    SECTION("deduce room") {
        REQUIRE_FALSE(notes[askPlayer][sugPlayer].seen);
        REQUIRE_FALSE(notes[askPlayer][sugWeapon].seen);
        REQUIRE_FALSE(notes[askPlayer][sugRoom].seen);

        notes[showPlayer][sugPlayer].lacks = true;
        notes[showPlayer][sugWeapon].lacks = true;

        log.addSuggestion(askPlayer, Bot::Suggestion(sugPlayer, sugWeapon, sugRoom));
        log.addShow(showPlayer);

        REQUIRE(deductor.run(log, notes));

        REQUIRE_FALSE(notes[askPlayer][sugPlayer].seen);
        REQUIRE_FALSE(notes[askPlayer][sugWeapon].seen);
        REQUIRE(notes[askPlayer][sugRoom].seen);

        REQUIRE_FALSE(deductor.run(log, notes));
    }

    sugPlayer = Bot::Player(int(sugPlayer) + 1);
    sugWeapon = Bot::Weapon(int(sugWeapon) + 1);
    sugRoom = Bot::Room(int(sugRoom) + 1);

    SECTION("deduce weapon") {
        REQUIRE_FALSE(notes[askPlayer][sugPlayer].seen);
        REQUIRE_FALSE(notes[askPlayer][sugWeapon].seen);
        REQUIRE_FALSE(notes[askPlayer][sugRoom].seen);

        notes[showPlayer][sugPlayer].lacks = true;
        notes[showPlayer][sugRoom].lacks = true;

        log.addSuggestion(askPlayer, Bot::Suggestion(sugPlayer, sugWeapon, sugRoom));
        log.addShow(showPlayer);

        REQUIRE(deductor.run(log, notes));

        REQUIRE_FALSE(notes[askPlayer][sugPlayer].seen);
        REQUIRE(notes[askPlayer][sugWeapon].seen);
        REQUIRE_FALSE(notes[askPlayer][sugRoom].seen);

        REQUIRE_FALSE(deductor.run(log, notes));
    }

    sugPlayer = Bot::Player(int(sugPlayer) + 1);
    sugWeapon = Bot::Weapon(int(sugWeapon) + 1);
    sugRoom = Bot::Room(int(sugRoom) + 1);

    SECTION("deduce player") {
        REQUIRE_FALSE(notes[askPlayer][sugPlayer].seen);
        REQUIRE_FALSE(notes[askPlayer][sugWeapon].seen);
        REQUIRE_FALSE(notes[askPlayer][sugRoom].seen);

        notes[showPlayer][sugWeapon].lacks = true;
        notes[showPlayer][sugRoom].lacks = true;

        log.addSuggestion(askPlayer, Bot::Suggestion(sugPlayer, sugWeapon, sugRoom));
        log.addShow(showPlayer);

        REQUIRE(deductor.run(log, notes));

        REQUIRE(notes[askPlayer][sugPlayer].seen);
        REQUIRE_FALSE(notes[askPlayer][sugWeapon].seen);
        REQUIRE_FALSE(notes[askPlayer][sugRoom].seen);

        REQUIRE_FALSE(deductor.run(log, notes));
    }
}

// vim: set expandtab textwidth=100:
