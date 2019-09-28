#include <catch/catch.hpp>
#include <algorithm>
#include "../include/bot.h"
#include "../include/tests.h"
#include "../include/deck.h"

using namespace AI;
using Catch::Matchers::Equals;

template <typename T>
bool contains(std::vector<T> vec, T obj)
{
    return std::find(vec.begin(), vec.end(), obj) != vec.end();
}

TEST_CASE("Suggestion struct", "[suggestion]") {
    auto p = randEnum(Bot::MAX_PLAYER);
    auto w = randEnum(Bot::MAX_WEAPON);
    auto r = randEnum(Bot::MAX_ROOM);
    Bot::Suggestion sug(p, w, r);
    REQUIRE(sug.player == p);
    REQUIRE(sug.weapon == w);
    REQUIRE(sug.room == r);
}

TEST_CASE("Card class", "[card]") {
    // test player
    auto p = randEnum(Bot::MAX_PLAYER);
    Bot::Card player(p);
    REQUIRE(player.type == Bot::Card::Type::PLAYER);
    REQUIRE(Bot::Player(player.card) == p);

    // test weapon
    auto w = randEnum(Bot::MAX_WEAPON);
    Bot::Card weapon(w);
    REQUIRE(weapon.type == Bot::Card::Type::WEAPON);
    REQUIRE(Bot::Weapon(weapon.card) == w);

    // test room
    auto r = randEnum(Bot::MAX_ROOM);
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
        Bot::Player from = randEnum(Bot::MAX_PLAYER);
        Bot::Player show = randEnum(Bot::MAX_PLAYER);
        Bot::Suggestion sug(randEnum(Bot::MAX_PLAYER), randEnum(Bot::MAX_WEAPON),
                randEnum(Bot::MAX_ROOM));

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

        Bot bot(player, { Bot::SCARLET, Bot::PLUM });

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

        Bot bot(player, { Bot::SCARLET, Bot::PLUM });

        Bot::Card card(weapon);
        bot.showCard(other, card);

        auto notes = bot.getNotes();

        REQUIRE(notes[player][card].seen);
        REQUIRE(notes[other][card].has);
    }

    /**
     * This section is specifically intended to test private functionality of the Bot class that
     * would otherwise be very difficult to consistently test only through the Bot class' public
     * members. Only functionality that is critical to the correct functioning of the rest of the
     * class is tested here to minimize testing of private members.
     */
    SECTION("private") {
        /**
         * By inheriting from the Bot class and changing the access specifier to public for select
         * members we are able to access private members from the unit tests.
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
                using Bot::getWantedDeck;

                using Bot::player;
                using Bot::notes;
                using Bot::order;
                using Bot::envelope;
        };

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

            SECTION("all-lacks") {
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

            SECTION("no-has") {
                SECTION("find player") {
                    BotTest bot(player, order);
                    Bot::Player ep = Bot::SCARLET;

                    REQUIRE_FALSE(bot.envelope.havePlayer);

                    for (int i = 0; i <= int(Bot::MAX_PLAYER); i++) {
                        if (Bot::Player(i) == ep)
                            continue;
                        bot.notes[order[rand() % order.size()]][Bot::Player(i)].has = true;
                    }

                    REQUIRE(bot.findEnvelope());
                    REQUIRE(bot.envelope.havePlayer);
                    REQUIRE(bot.envelope.player == ep);

                    REQUIRE_FALSE(bot.findEnvelope());
                }

                SECTION("find weapon") {
                    BotTest bot(player, order);
                    Bot::Weapon ew = Bot::CANDLESTICK;

                    REQUIRE_FALSE(bot.envelope.haveWeapon);

                    for (int i = 0; i <= int(Bot::MAX_WEAPON); i++) {
                        if (Bot::Weapon(i) == ew)
                            continue;
                        bot.notes[order[rand() % order.size()]][Bot::Weapon(i)].has = true;
                    }

                    REQUIRE(bot.findEnvelope());
                    REQUIRE(bot.envelope.haveWeapon);
                    REQUIRE(bot.envelope.weapon == ew);

                    REQUIRE_FALSE(bot.findEnvelope());
                }

                SECTION("find room") {
                    BotTest bot(player, order);
                    Bot::Room er = Bot::BEDROOM;

                    REQUIRE_FALSE(bot.envelope.haveRoom);

                    for (int i = 0; i <= int(Bot::MAX_ROOM); i++) {
                        if (Bot::Room(i) == er)
                            continue;
                        bot.notes[order[rand() % order.size()]][Bot::Room(i)].has = true;
                    }

                    REQUIRE(bot.findEnvelope());
                    REQUIRE(bot.envelope.haveRoom);
                    REQUIRE(bot.envelope.room == er);

                    REQUIRE_FALSE(bot.findEnvelope());
                }

                SECTION("multiple no-has") {
                    BotTest bot(player, order);
                    Bot::Player ep1 = Bot::SCARLET;
                    Bot::Player ep2 = Bot::PLUM;

                    REQUIRE_FALSE(bot.envelope.havePlayer);

                    for (int i = 0; i <= int(Bot::MAX_PLAYER); i++) {
                        if ((Bot::Player(i) == ep1) || (Bot::Player(i) == ep2))
                            continue;
                        bot.notes[order[rand() % order.size()]][Bot::Player(i)].has = true;
                    }

                    REQUIRE_FALSE(bot.findEnvelope());
                    REQUIRE_FALSE(bot.envelope.havePlayer);
                }
            }
        }

        SECTION("getWantedDeck") {
            BotTest bot(player, order);

            std::vector<Bot::Player> wantedPlayers = { Bot::SCARLET, Bot::PEACOCK, Bot::WHITE };
            std::vector<Bot::Weapon> wantedWeapons = { Bot::CANDLESTICK, Bot::LEAD_PIPE, Bot::SPANNER };
            std::vector<Bot::Room> wantedRooms = { Bot::BEDROOM, Bot::DINING_ROOM, Bot::GAMES_ROOM };

            for (int i = 0; i <= int(Bot::MAX_PLAYER); i++)
                if (!contains(wantedPlayers, Bot::Player(i)))
                    bot.notes[order[rand() % order.size()]][Bot::Player(i)].has = true;
            for (int i = 0; i <= int(Bot::MAX_WEAPON); i++)
                if (!contains(wantedWeapons, Bot::Weapon(i)))
                    bot.notes[order[rand() % order.size()]][Bot::Weapon(i)].has = true;
            for (int i = 0; i <= int(Bot::MAX_ROOM); i++)
                if (!contains(wantedRooms, Bot::Room(i)))
                    bot.notes[order[rand() % order.size()]][Bot::Room(i)].has = true;

            Deck deck = bot.getWantedDeck();

            REQUIRE_THAT(deck.players, Equals(wantedPlayers));
            REQUIRE_THAT(deck.weapons, Equals(wantedWeapons));
            REQUIRE_THAT(deck.rooms, Equals(wantedRooms));

            bot.envelope.havePlayer = true;
            deck = bot.getWantedDeck();
            REQUIRE(deck.players.empty());
            REQUIRE_THAT(deck.weapons, Equals(wantedWeapons));
            REQUIRE_THAT(deck.rooms, Equals(wantedRooms));

            bot.envelope.havePlayer = false;
            bot.envelope.haveWeapon = true;
            deck = bot.getWantedDeck();
            REQUIRE(deck.weapons.empty());
            REQUIRE_THAT(deck.players, Equals(wantedPlayers));
            REQUIRE_THAT(deck.rooms, Equals(wantedRooms));

            bot.envelope.haveWeapon = false;
            bot.envelope.haveRoom = true;
            deck = bot.getWantedDeck();
            REQUIRE(deck.rooms.empty());
            REQUIRE_THAT(deck.players, Equals(wantedPlayers));
            REQUIRE_THAT(deck.weapons, Equals(wantedWeapons));
        }
    }
}

// vim: set expandtab textwidth=100:
