#include <catch/catch.hpp>
#include "../include/deck.h"

using namespace AI;
using Catch::Matchers::Equals;

TEST_CASE("Deck struct", "[deck]") {
    SECTION("sort") {
        Deck deck;

        deck.players = { Bot::SCARLET, Bot::PLUM, Bot::PEACOCK };
        deck.weapons = { Bot::CANDLESTICK, Bot::KNIFE, Bot::LEAD_PIPE };
        deck.rooms = { Bot::BEDROOM, Bot::BATHROOM, Bot::STUDY };

        deck.scores[Bot::SCARLET] = 0;
        deck.scores[Bot::PLUM] = -1;
        deck.scores[Bot::PEACOCK] = 1;

        deck.scores[Bot::CANDLESTICK] = 0;
        deck.scores[Bot::KNIFE] = -1;
        deck.scores[Bot::LEAD_PIPE] = 1;

        deck.scores[Bot::BEDROOM] = 0;
        deck.scores[Bot::BATHROOM] = -1;
        deck.scores[Bot::STUDY] = 1;

        REQUIRE_NOTHROW(deck.sort());

        REQUIRE_THAT(deck.players, Equals(std::vector<Bot::Player>({ Bot::PLUM, Bot::SCARLET,
                        Bot::PEACOCK })));
        REQUIRE_THAT(deck.weapons, Equals(std::vector<Bot::Weapon>({ Bot::KNIFE, Bot::CANDLESTICK,
                        Bot::LEAD_PIPE })));
        REQUIRE_THAT(deck.rooms, Equals(std::vector<Bot::Room>({ Bot::BATHROOM, Bot::BEDROOM,
                        Bot::STUDY})));
    }
}

// vim: set expandtab textwidth=100:
