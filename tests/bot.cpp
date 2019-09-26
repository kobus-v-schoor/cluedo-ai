#include <catch/catch.hpp>
#include "../include/bot.h"
#include "../include/tests.h"

using namespace AI;
using Catch::Matchers::Equals;

/**
 * Class used to allow access to private members to allow testing of some private
 * functionality which would be very difficult to test using just the public members of the Bot
 * class
 */
class BotTest : public Bot {
    public:
        BotTest(Player p, std::vector<Player> o) :
            Bot::Bot(p, o)
        {}

        using Bot::Notes;
        using Bot::Envelope;

        using Bot::notesMarkLacking;
        using Bot::findEnvelope;

        using Bot::player;
        using Bot::notes;
        using Bot::order;
        using Bot::envelope;
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
    SECTION("Notes struct") {
        Bot::Notes notes;
        REQUIRE_FALSE(notes.has);
        REQUIRE_FALSE(notes.seen);
        REQUIRE_FALSE(notes.lacks);
        REQUIRE_FALSE(notes.deduced);

        notes.has = true;
        REQUIRE(notes.concluded());
        REQUIRE(notes.knows());

        notes.has = false;
        notes.lacks = true;
        REQUIRE(notes.concluded());
        REQUIRE_FALSE(notes.knows());

        notes.seen = true;
        REQUIRE(notes.concluded());
        REQUIRE(notes.knows());
    }

    SECTION("SuggestionLog class") {
        Bot::Player from = randEnum(Bot::Player::WHITE);
        Bot::Player show = randEnum(Bot::Player::WHITE);
        Bot::Suggestion sug(randEnum(Bot::Player::WHITE), randEnum(Bot::Weapon::SPANNER),
                randEnum(Bot::Room::GAMES_ROOM));

        Bot::SuggestionLog log;

        // test adding a suggestion
        REQUIRE_NOTHROW(log.addSuggestion(from, sug));
        REQUIRE_NOTHROW(log.addShow(show));
        REQUIRE(log.log().size() == 1);

        // testing adding a second, no show suggestion
        REQUIRE_NOTHROW(log.addSuggestion(from, sug));
        REQUIRE_NOTHROW(log.addNoShow());
        REQUIRE(log.log().size() == 2);

        Bot::SuggestionLogItem item = log.log()[0];

        // testing if suggestion was correctly added
        REQUIRE(item.suggestion == sug);
        REQUIRE(item.from == from);
        REQUIRE(item.show == show);
        REQUIRE(item.showed);

        // testing that second suggestion was correctly added
        REQUIRE_FALSE(log.log()[1].showed);

        // testing exceptions
        REQUIRE_THROWS_AS(log.addShow(show), std::runtime_error&);
        REQUIRE_NOTHROW(log.addSuggestion(from, sug));
        REQUIRE_THROWS_AS(log.addSuggestion(from, sug), std::runtime_error&);

        // testing clearing
        REQUIRE(log.waiting());
        log.clear();
        REQUIRE_FALSE(log.waiting());
    }

    SECTION("set cards") {
        Bot::Player player = Bot::SCARLET;
        Bot::Weapon weapon = Bot::CANDLESTICK;

        Bot bot(player, {});

        Bot::Card card(weapon);
        bot.setCards({ card });

        auto notes = bot.getNotes();

        REQUIRE(notes[player][weapon].has);
        REQUIRE_FALSE(notes[player][weapon].lacks);
    }

    SECTION("showing card to bot") {
        Bot::Player player = Bot::SCARLET;
        Bot::Player other = Bot::PLUM;
        Bot::Weapon weapon = Bot::CANDLESTICK;

        Bot bot(player, {});

        Bot::Card card(weapon);
        bot.showCard(other, card);

        auto notes = bot.getNotes();

        REQUIRE(notes[player][card].seen);
        REQUIRE(notes[other][card].has);
    }

    // this is the only section that should use the BotTest class - this section will test private
    // functionality.
    // This section should only test the bare minimum to minimize testing of private members
    SECTION("private") {
        Bot::Player player = Bot::Player::SCARLET;
        std::vector<Bot::Player> order = { Bot::Player::SCARLET, Bot::Player::PLUM,
            Bot::Player::PEACOCK , Bot::Player::GREEN };


        SECTION("init") {
            BotTest bot(player, order);

            REQUIRE(bot.player == player);
            REQUIRE_THAT(bot.order, Equals(order));
            // check that a notes entry was created for every player
            REQUIRE(bot.notes.size() == bot.order.size());
        }

        SECTION("Envelope struct") {
            BotTest::Envelope env;
            REQUIRE_FALSE(env.havePlayer);
            REQUIRE_FALSE(env.haveWeapon);
            REQUIRE_FALSE(env.haveRoom);

            BotTest::Envelope other = env;
            REQUIRE_FALSE(env != other);

            other.havePlayer = true;
            REQUIRE(env != other);
            env.havePlayer = true;
            REQUIRE_FALSE(env != other);

            other.haveWeapon = true;
            REQUIRE(env != other);
            env.haveWeapon = true;
            REQUIRE_FALSE(env != other);

            other.haveRoom = true;
            REQUIRE(env != other);
            env.haveRoom = true;
            REQUIRE_FALSE(env != other);
        }

        SECTION("notesMarkLacking") {
            BotTest bot(player, order);

            Bot::Card card(Bot::SCARLET);

            bot.notes[player][card].lacks = false;
            for (auto p : order)
                REQUIRE_FALSE(bot.notes[p][card].lacks);

            bot.notes[player][card].has = true;
            bot.notesMarkLacking();
            for (auto p : order) {
                if (p == player)
                    REQUIRE_FALSE(bot.notes[p][card].lacks);
                else
                    REQUIRE(bot.notes[p][card].lacks);
            }
        }

        SECTION("findEnvelope") {
            SECTION("sanity check") {
                BotTest bot(player, order);
                REQUIRE_FALSE(bot.findEnvelope());
            }

            SECTION("find player") {
                BotTest bot(player, order);
                Bot::Player ep = Bot::SCARLET;

                REQUIRE_FALSE(bot.envelope.havePlayer);

                for (auto p : order)
                    bot.notes[p][ep].lacks = true;

                REQUIRE(bot.findEnvelope());
                REQUIRE(bot.envelope.havePlayer);
                REQUIRE(bot.envelope.player == ep);

                REQUIRE_FALSE(bot.findEnvelope());
            }

            SECTION("find weapon") {
                BotTest bot(player, order);
                Bot::Weapon ew = Bot::CANDLESTICK;

                REQUIRE_FALSE(bot.envelope.haveWeapon);

                for (auto p : order)
                    bot.notes[p][ew].lacks = true;

                REQUIRE(bot.findEnvelope());
                REQUIRE(bot.envelope.haveWeapon);
                REQUIRE(bot.envelope.weapon == ew);

                REQUIRE_FALSE(bot.findEnvelope());
            }

            SECTION("find room") {
                BotTest bot(player, order);
                Bot::Room er = Bot::BEDROOM;

                REQUIRE_FALSE(bot.envelope.haveRoom);

                for (auto p : order)
                    bot.notes[p][er].lacks = true;

                REQUIRE(bot.findEnvelope());
                REQUIRE(bot.envelope.haveRoom);
                REQUIRE(bot.envelope.room == er);

                REQUIRE_FALSE(bot.findEnvelope());
            }

            SECTION("one has") {
                BotTest bot(player, order);
                Bot::Player ep = Bot::SCARLET;

                for (auto p : order)
                    bot.notes[p][ep].lacks = true;

                Bot::Player chosen = order[rand() % order.size()];
                bot.notes[chosen][ep].lacks = false;
                bot.notes[chosen][ep].has = true;

                REQUIRE_FALSE(bot.findEnvelope());
                REQUIRE_FALSE(bot.envelope.havePlayer);
            }
        }
    }
}

// vim: set expandtab textwidth=100:
