#include <catch/catch.hpp>
#include "../../include/predictors/multiple.h"

using namespace AI;

TEST_CASE("MultiplePredictor", "[multiple-predictor]") {
    std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>> notes;
    Bot::SuggestionLog log;
    Deck deck;
    Bot::Player sugPlayer = Bot::SCARLET;
    Bot::Weapon card = Bot::CANDLESTICK;
    MultiplePredictor predictor(sugPlayer);

    log.addSuggestion(sugPlayer, Bot::Suggestion(Bot::Player(0), card, Bot::Room(0)));
    log.addNoShow();

    SECTION("not in deck") {
        predictor.run(deck, notes, log);
        REQUIRE(deck.scores[card] == 0);
    }

    SECTION("doesn't lack") {
        deck.weapons.push_back(card);

        predictor.run(deck, notes, log);
        REQUIRE(deck.scores[card] == 0);
    }

    SECTION("not enough suggestions") {
        deck.weapons.push_back(card);
        notes[sugPlayer][card].lacks = true;

        predictor.run(deck, notes, log);
        REQUIRE(deck.scores[card] == 0);
    }

    SECTION("in deck, lacks and suggested multiple times") {
        deck.weapons.push_back(card);
        notes[sugPlayer][card].lacks = true;
        log.addSuggestion(sugPlayer, Bot::Suggestion(Bot::Player(0), card, Bot::Room(0)));
        log.addNoShow();

        predictor.run(deck, notes, log);
        REQUIRE(deck.scores[card] == -1);
    }
}

// vim: set expandtab textwidth=100:
