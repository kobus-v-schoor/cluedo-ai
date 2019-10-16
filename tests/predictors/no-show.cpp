#include <catch/catch.hpp>
#include "../../include/predictors/no-show.h"

using namespace AI;

TEST_CASE("NoShowPredictor", "[no-show-predictor]") {
    std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>> notes;
    Bot::SuggestionLog log;
    Deck deck;
    Bot::Player askPlayer = Bot::SCARLET;
    NoShowPredictor predictor(askPlayer);

    Bot::Player sugPlayer = Bot::SCARLET;
    Bot::Weapon sugWeapon = Bot::CANDLESTICK;
    Bot::Room sugRoom = Bot::BEDROOM;

    SECTION("card not in deck") {
        notes[askPlayer][sugPlayer].has = true;
        notes[askPlayer][sugWeapon].has = true;

        log.addSuggestion(askPlayer, Bot::Suggestion(sugPlayer, sugWeapon, sugRoom));
        log.addNoShow();

        REQUIRE_NOTHROW(predictor.run(deck, notes, log));

        REQUIRE(deck.scores[sugRoom] == 0);
        REQUIRE(deck.scores[sugPlayer] == 0);
        REQUIRE(deck.scores[sugWeapon] == 0);
    }

    SECTION("mark room") {
        notes[askPlayer][sugPlayer].has = true;
        notes[askPlayer][sugWeapon].has = true;

        deck.rooms.push_back(sugRoom);

        log.addSuggestion(askPlayer, Bot::Suggestion(sugPlayer, sugWeapon, sugRoom));
        log.addNoShow();

        REQUIRE_NOTHROW(predictor.run(deck, notes, log));

        REQUIRE(deck.scores[sugRoom] < 0);
        REQUIRE(deck.scores[sugPlayer] == 0);
        REQUIRE(deck.scores[sugWeapon] == 0);
    }

    SECTION("mark weapon") {
        notes[askPlayer][sugPlayer].has = true;
        notes[askPlayer][sugRoom].has = true;

        deck.weapons.push_back(sugWeapon);

        log.addSuggestion(askPlayer, Bot::Suggestion(sugPlayer, sugWeapon, sugRoom));
        log.addNoShow();

        REQUIRE_NOTHROW(predictor.run(deck, notes, log));

        REQUIRE(deck.scores[sugWeapon] < 0);
        REQUIRE(deck.scores[sugPlayer] == 0);
        REQUIRE(deck.scores[sugRoom] == 0);
    }

    SECTION("mark player") {
        notes[askPlayer][sugWeapon].has = true;
        notes[askPlayer][sugRoom].has = true;

        deck.players.push_back(sugPlayer);

        log.addSuggestion(askPlayer, Bot::Suggestion(sugPlayer, sugWeapon, sugRoom));
        log.addNoShow();

        REQUIRE_NOTHROW(predictor.run(deck, notes, log));

        REQUIRE(deck.scores[sugPlayer] < 0);
        REQUIRE(deck.scores[sugWeapon] == 0);
        REQUIRE(deck.scores[sugRoom] == 0);
    }
}

// vim: set expandtab textwidth=100:
