#include <catch/catch.hpp>
#include "../../include/predictors/seen.h"

using namespace AI;

TEST_CASE("SeenPredictor", "[seen-predictor]") {
    std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>> notes;
    Bot::SuggestionLog log;
    Deck deck;
    SeenPredictor predictor;

    Bot::Player askPlayer = Bot::SCARLET;
    Bot::Player showPlayer = Bot::PLUM;

    Bot::Player sugPlayer = Bot::SCARLET;
    Bot::Weapon sugWeapon = Bot::CANDLESTICK;
    Bot::Room sugRoom = Bot::BEDROOM;

    SECTION("card not in deck") {
        notes[askPlayer][sugPlayer].seen = true;
        notes[askPlayer][sugWeapon].seen = true;

        log.addSuggestion(askPlayer, Bot::Suggestion(sugPlayer, sugWeapon, sugRoom));
        log.addShow(showPlayer);

        REQUIRE_NOTHROW(predictor.run(deck, notes, log));

        REQUIRE(deck.scores[sugRoom] == 0);
    }

    sugPlayer = Bot::Player(int(sugPlayer) + 1);
    sugWeapon = Bot::Weapon(int(sugWeapon) + 1);
    sugRoom = Bot::Room(int(sugRoom) + 1);

    SECTION("mark room") {
        notes[askPlayer][sugPlayer].seen = true;
        notes[askPlayer][sugWeapon].seen = true;
        deck.rooms.push_back(sugRoom);

        log.addSuggestion(askPlayer, Bot::Suggestion(sugPlayer, sugWeapon, sugRoom));
        log.addShow(showPlayer);

        REQUIRE_NOTHROW(predictor.run(deck, notes, log));

        REQUIRE(deck.scores[sugRoom] == 1);
    }

    sugPlayer = Bot::Player(int(sugPlayer) + 1);
    sugWeapon = Bot::Weapon(int(sugWeapon) + 1);
    sugRoom = Bot::Room(int(sugRoom) + 1);

    SECTION("mark weapon") {
        notes[askPlayer][sugPlayer].seen = true;
        notes[askPlayer][sugRoom].seen = true;
        deck.weapons.push_back(sugWeapon);

        log.addSuggestion(askPlayer, Bot::Suggestion(sugPlayer, sugWeapon, sugRoom));
        log.addShow(showPlayer);

        REQUIRE_NOTHROW(predictor.run(deck, notes, log));

        REQUIRE(deck.scores[sugWeapon] == 1);
    }

    sugPlayer = Bot::Player(int(sugPlayer) + 1);
    sugWeapon = Bot::Weapon(int(sugWeapon) + 1);
    sugRoom = Bot::Room(int(sugRoom) + 1);

    SECTION("mark player") {
        notes[askPlayer][sugWeapon].seen = true;
        notes[askPlayer][sugRoom].seen = true;
        deck.players.push_back(sugPlayer);

        log.addSuggestion(askPlayer, Bot::Suggestion(sugPlayer, sugWeapon, sugRoom));
        log.addShow(showPlayer);

        REQUIRE_NOTHROW(predictor.run(deck, notes, log));

        REQUIRE(deck.scores[sugPlayer] == 1);
    }
}

// vim: set expandtab textwidth=100:
