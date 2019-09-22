#include <catch/catch.hpp>
#include "../include/bot.h"
#include "../include/tests.h"

using namespace AI;

/**
 * Class used to allow access to private members to allow testing of some private
 * functionality which would be very difficult to test using just the public members of the Bot
 * class
 */
class BotTest : public Bot {
    public:
        BotTest(Player p) :
            Bot::Bot(p)
        {}

        using Bot::Notes;

        using Bot::player;
        using Bot::notes;
};

TEST_CASE("Suggestion struct", "[suggestion]") {
    auto p = randEnum(Bot::Player::WHITE);
    auto w = randEnum(Bot::Weapon::SPANNER);
    auto r = randEnum(Bot::Room::GAMES_ROOM);
    Bot::Suggestion sug(p, w, r);
    REQUIRE(sug.player == p);
    REQUIRE(sug.weapon == w);
    REQUIRE(sug.room == r);
}

TEST_CASE("Card class", "[card]") {
    // test player
    auto p = randEnum(Bot::Player::WHITE);
    Bot::Card player(p);
    REQUIRE(player.type == Bot::Card::Type::PLAYER);
    REQUIRE(Bot::Player(player.card) == p);

    // test weapon
    auto w = randEnum(Bot::Weapon::SPANNER);
    Bot::Card weapon(w);
    REQUIRE(weapon.type == Bot::Card::Type::WEAPON);
    REQUIRE(Bot::Weapon(weapon.card) == w);

    // test room
    auto r = randEnum(Bot::Room::GAMES_ROOM);
    Bot::Card room(r);
    REQUIRE(room.type == Bot::Card::Type::ROOM);
    REQUIRE(Bot::Room(room.card) == r);
}

TEST_CASE("Bot class", "[bot]") {
    // this is the only section that should use the BotTest class - this section will test private
    // functionality.
    // This section should only test the bare minimum to minimize testing of private members
    SECTION("private") {
        SECTION("Notes struct") {
            BotTest::Notes notes;
            REQUIRE_FALSE(notes.dealt);
            REQUIRE_FALSE(notes.shown);
            REQUIRE_FALSE(notes.seen);
        }

        Bot::Player player = randEnum(Bot::Player::WHITE);
        BotTest bot(player);

        REQUIRE(bot.player == player);

        SECTION("show and set cards") {
            Bot::Card sc(randEnum(Bot::Weapon::SPANNER));
            bot.setCards({ sc });
            REQUIRE(bot.notes[player][sc].dealt);

            Bot::Player other = randEnum(Bot::Player::WHITE);
            bot.showCard(other, sc);
            REQUIRE(bot.notes[player][sc].seen);
            REQUIRE(bot.notes[other][sc].dealt);
            REQUIRE(bot.notes[other][sc].shown);
        }
    }
}

// vim: set expandtab textwidth=100:
