#include <catch/catch.hpp>
#include <algorithm>
#include "../include/bot.h"
#include "../include/tests.h"
#include "../include/deck.h"

using namespace AI;
using Catch::Matchers::Equals;
using Catch::Matchers::VectorContains;

template <typename T>
bool contains(std::vector<T> vec, T obj)
{
    return std::find(vec.begin(), vec.end(), obj) != vec.end();
}

Bot::Room posToRoom(int pos)
{
    switch (pos) {
        case 1: return Bot::COURTYARD;
        case 2: return Bot::GARAGE;
        case 3: return Bot::GAMES_ROOM;
        case 4: return Bot::BEDROOM;
        case 5: return Bot::BATHROOM;
        case 6: return Bot::STUDY;
        case 7: return Bot::KITCHEN;
        case 8: return Bot::DINING_ROOM;
        case 9: return Bot::LIVING_ROOM;
    };

    return Bot::COURTYARD;
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

    SECTION("getMove and getSuggestion") {
        Bot::Player player = Bot::SCARLET;
        Bot::Player other1 = Bot::PLUM;
        Bot::Player other2 = Bot::PEACOCK;
        std::vector<Bot::Player> order = { player, other1, other2 };
        Bot bot(player, order);

        SECTION("incorrect use") {
            REQUIRE_THROWS_AS(bot.getSuggestion(), std::runtime_error&);
        }

        SECTION("room") {
            std::vector<Bot::Room> wanted = { Bot::DINING_ROOM, Bot::STUDY };

            std::vector<Bot::Card> safePlayers = { Bot::PLUM, Bot::GREEN };
            std::vector<Bot::Card> safeWeapons = { Bot::CANDLESTICK, Bot::LEAD_PIPE, Bot::ROPE };

            for (int i = 0; i <= int(Bot::MAX_ROOM); i++)
                if (!contains(wanted,  Bot::Room(i)))
                    bot.setCards({ Bot::Room(i) });

            bot.setCards(safePlayers);
            bot.setCards(safeWeapons);

            Bot::Suggestion sug(Bot::Player(0), Bot::Weapon(0), Bot::Room(0));

            SECTION("enter wanted room") {
                bot.updateBoard({
                        { player, 22 },
                        { other1, 26 },
                        { other2, 27 }
                        });

                REQUIRE(bot.getMove(4) == 8);
                REQUIRE_NOTHROW(sug = bot.getSuggestion());
                REQUIRE(sug.room == Bot::DINING_ROOM);
                REQUIRE_THAT(safePlayers, VectorContains(Bot::Card(sug.player)));
                REQUIRE_THAT(safeWeapons, VectorContains(Bot::Card(sug.weapon)));
            }

            SECTION("enter unwanted room") {
                bot.updateBoard({
                        { player, 70 },
                        { other1, 19 },
                        { other2, 27 }
                        });

                bot.setCards(safePlayers);
                bot.setCards(safeWeapons);

                REQUIRE(bot.getMove(4) == 3);
                REQUIRE_NOTHROW(sug = bot.getSuggestion());
                REQUIRE(sug.room == Bot::GAMES_ROOM);
                REQUIRE(!contains(safePlayers, Bot::Card(sug.player)));
                REQUIRE(!contains(safeWeapons, Bot::Card(sug.weapon)));
            }

            SECTION("enter tile") {
                bot.updateBoard({
                        { player, 30 },
                        { other1, 19 },
                        { other2, 27 }
                        });

                REQUIRE(bot.getMove(2) == 13);
                REQUIRE_THROWS_AS(bot.getSuggestion(), std::runtime_error&);
            }
        }

        SECTION("player and weapon") {
            Bot::Player player = Bot::SCARLET;
            Bot::Player other1 = Bot::PLUM;
            Bot::Player other2 = Bot::PEACOCK;
            std::vector<Bot::Player> order = { player, other1, other2 };
            Bot bot(player, order);

            Bot::Room envelopeRoom = Bot::DINING_ROOM;
            std::vector<Bot::Card> safeRooms = { Bot::STUDY, Bot::LIVING_ROOM };
            std::vector<Bot::Card> safePlayers = { Bot::PLUM, Bot::GREEN };
            std::vector<Bot::Card> safeWeapons = { Bot::CANDLESTICK, Bot::LEAD_PIPE, Bot::ROPE };

            for (int i = 0; i <= int(Bot::MAX_ROOM); i++)
                if (Bot::Room(i) != envelopeRoom)
                    bot.showCard(other1, Bot::Room(i));

            bot.setCards(safeRooms);
            bot.setCards(safePlayers);
            bot.setCards(safeWeapons);

            Bot::Suggestion sug(Bot::Player(0), Bot::Weapon(0), Bot::Room(0));

            SECTION("stay in safe") {
                bot.updateBoard({
                        { player, 9 },
                        { other1, 38 },
                        { other2, 39 }
                        });

                REQUIRE(bot.getMove(7) == 9);
            }

            SECTION("go from envelope to safe") {
                bot.updateBoard({
                        { player, 8 },
                        { other1, 38 },
                        { other2, 39 }
                        });

                REQUIRE(bot.getMove(7) == 9);
            }

            SECTION("stay in envelope") {
                bot.updateBoard({
                        { player, 8 },
                        { other1, 38 },
                        { other2, 39 }
                        });

                REQUIRE(bot.getMove(3) == 8);
            }

            SECTION("go to safe") {
                SECTION("from room") {
                    bot.updateBoard({
                            { player, 5 },
                            { other1, 38 },
                            { other2, 39 }
                            });

                    REQUIRE(bot.getMove(3) == 6);
                }

                SECTION("from tile") {
                    bot.updateBoard({
                            { player, 54 },
                            { other1, 38 },
                            { other2, 39 }
                            });

                    REQUIRE(bot.getMove(5) == 6);
                }
            }

            SECTION("cannot reach safe room");
            SECTION("player");
            SECTION("weapon");
        }

        SECTION("accusation");
    }

    /**
     * This section is specifically intended to test private functionality of the Bot class that
     * would otherwise be very difficult to consistently test only through the Bot class' public
     * interface. Private variables should not be tested (except for the init section), only private
     * functions.
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
                using Bot::getRoomPos;
                using Bot::getPosRoom;
                using Bot::findNextMove;
                using Bot::choosePlayerOffensive;

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

        SECTION("getRoomPos") {
            BotTest bot(player, order);

            REQUIRE(bot.getRoomPos(Bot::BEDROOM) == 4);
            REQUIRE(bot.getRoomPos(Bot::BATHROOM) == 5);
            REQUIRE(bot.getRoomPos(Bot::STUDY) == 6);
            REQUIRE(bot.getRoomPos(Bot::KITCHEN) == 7);
            REQUIRE(bot.getRoomPos(Bot::DINING_ROOM) == 8);
            REQUIRE(bot.getRoomPos(Bot::LIVING_ROOM) == 9);
            REQUIRE(bot.getRoomPos(Bot::COURTYARD) == 1);
            REQUIRE(bot.getRoomPos(Bot::GARAGE) == 2);
            REQUIRE(bot.getRoomPos(Bot::GAMES_ROOM) == 3);
        }

        SECTION("getPosRoom") {
            BotTest bot(player, order);

            REQUIRE(bot.getPosRoom(1) == Bot::COURTYARD);
            REQUIRE(bot.getPosRoom(2) == Bot::GARAGE);
            REQUIRE(bot.getPosRoom(3) == Bot::GAMES_ROOM);
            REQUIRE(bot.getPosRoom(4) == Bot::BEDROOM);
            REQUIRE(bot.getPosRoom(5) == Bot::BATHROOM);
            REQUIRE(bot.getPosRoom(6) == Bot::STUDY);
            REQUIRE(bot.getPosRoom(7) == Bot::KITCHEN);
            REQUIRE(bot.getPosRoom(8) == Bot::DINING_ROOM);
            REQUIRE(bot.getPosRoom(9) == Bot::LIVING_ROOM);
        }

        SECTION("findNextMove") {
            Bot::Player player = Bot::SCARLET;
            Bot::Player other1 = Bot::PLUM;
            Bot::Player other2 = Bot::PEACOCK;

            BotTest bot(player, { player, other1, other2 } );

            std::vector<Bot::Room> wanted;

            SECTION("already in wanted room") {
                wanted = { Bot::DINING_ROOM , Bot::KITCHEN };
                bot.updateBoard({
                        { player, 8 },
                        { other1, 33 },
                        { other2, 34 }
                        });

                REQUIRE(bot.findNextMove(12, wanted) == bot.getRoomPos(Bot::KITCHEN));
            }

            SECTION("unblocked wanted rooms") {
                SECTION("reachable") {
                    wanted = { Bot::DINING_ROOM, Bot::LIVING_ROOM, Bot::COURTYARD };
                    bot.updateBoard({
                            { player, 30 },
                            { other1, 34 },
                            { other2, 35 }
                            });

                    REQUIRE(bot.findNextMove(6, wanted) == bot.getRoomPos(Bot::DINING_ROOM));
                    REQUIRE(bot.findNextMove(5, wanted) == bot.getRoomPos(Bot::LIVING_ROOM));
                }

                SECTION("unreachable") {
                    SECTION("unwanted room reachable") {
                        wanted = { Bot::KITCHEN, Bot::DINING_ROOM };
                        bot.updateBoard({
                                { player, 48 },
                                { other1, 21 },
                                { other2, 35 }
                                });

                        REQUIRE(bot.findNextMove(10, wanted) == 2);
                    }

                    SECTION("no rooms reachable") {
                        wanted = { Bot::DINING_ROOM };
                        bot.updateBoard({
                                { player, 29 },
                                { other1, 21 },
                                { other2, 35 }
                                });

                        REQUIRE(int(Position(bot.findNextMove(2, wanted)).path(9)) == 2);
                    }
                }
            }

            SECTION("no unblocked wanted rooms") {
                SECTION("fenced in") {
                    wanted = { Bot::DINING_ROOM, Bot::LIVING_ROOM, Bot::COURTYARD };

                    SECTION("in room") {
                        bot.updateBoard({
                                { player, 3 },
                                { other1, 78 },
                                { other2, 77 }
                                });

                        REQUIRE(bot.findNextMove(12, wanted) == 3);
                    }

                    SECTION("on tiles") {
                        bot.updateBoard({
                                { player, 43 },
                                { other1, 33 },
                                { other2, 44 }
                                });

                        REQUIRE(bot.findNextMove(12, wanted) == 43);
                    }
                }

                SECTION("can reach unwanted room") {
                    SECTION("closest by tiles") {
                        wanted = { Bot::STUDY, Bot::GAMES_ROOM };
                        bot.updateBoard({
                                { player, 64 },
                                { other1, 41 },
                                { other2, 78 }
                                });

                        REQUIRE(bot.findNextMove(5, wanted) == 5);
                    }

                    SECTION("closest by shortcut") {
                        wanted = { Bot::STUDY, Bot::BEDROOM };
                        bot.updateBoard({
                                { player, 23 },
                                { other1, 41 },
                                { other2, 74 }
                                });

                        REQUIRE(bot.findNextMove(5, wanted) == 9);
                    }
                }

                SECTION("cannot reach any rooms") {
                    Bot::Player other3 = Bot::GREEN;
                    BotTest bot(player, { player, other1, other2, other3 });

                    wanted = { Bot::BEDROOM, Bot::STUDY };
                    bot.updateBoard({
                            { player, 64 },
                            { other1, 41 },
                            { other2, 51 },
                            { other3, 74 }
                            });

                    REQUIRE(bot.findNextMove(5, wanted) == 78);
                }
            }
        }

        SECTION("choosePlayerOffensive") {
            SECTION("no overlap") {
                std::vector<Bot::Player> choices = { Bot::MUSTARD, Bot::WHITE };
                BotTest bot(player, order);

                for (auto c : choices)
                    REQUIRE_FALSE(contains(order, c));

                // just make sure that returned player is always in choices, player doesn't matter
                // since output is random it is tested multiple times
                for (int i = 0; i < 10; i++)
                    REQUIRE_THAT(choices, VectorContains(bot.choosePlayerOffensive(choices,
                                    randEnum(Bot::MAX_ROOM))));
            }

            SECTION("no seen") {
                std::vector<Bot::Player> choices = { Bot::SCARLET, Bot::PLUM, Bot::PEACOCK };
                BotTest bot(player, order);

                Bot::Player min = choices[0];
                for (auto c : choices) {
                    for (int i = 0; i < int(c); i++)
                        bot.notes[c][Bot::Room(i)].seen = true;
                    if (int(c) < min)
                        min = c;
                }

                REQUIRE(bot.choosePlayerOffensive(choices, Bot::MAX_ROOM) == min);
            }

            SECTION("some seen") {
                std::vector<Bot::Player> choices = { Bot::SCARLET, Bot::PLUM, Bot::PEACOCK };
                BotTest bot(player, order);

                Bot::Player max = choices[0];
                for (size_t j = 0; j < choices.size(); j++) {
                    Bot::Player c = choices[j];
                    for (int i = 0; i < int(c); i++)
                        bot.notes[c][Bot::Room(i)].seen = true;
                    if (j + 1 < choices.size()) {
                        bot.notes[c][Bot::MAX_ROOM].seen = true;
                        if (int(c) > max)
                            max = c;
                    }
                }

                REQUIRE(bot.choosePlayerOffensive(choices, Bot::MAX_ROOM) == max);
            }
        }
    }
}

// vim: set expandtab textwidth=100:
