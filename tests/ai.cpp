#include <catch/catch.hpp>
#include <time.h>
#include "../include/ai.h"

template <typename T>
T rand_enum(T max_enum) {
    return T(rand() % (int(max_enum) + 1));
}

TEST_CASE("ai aux classes", "[aux]") {
    srand(time(NULL));

    SECTION("card class") {
        // test player
        auto p = rand_enum(AI::Player::WHITE);
        AI::Card player(p);
        REQUIRE(player.type == AI::Card::Type::PLAYER);
        REQUIRE(AI::Player(player.card) == p);

        // test weapon
        auto w = rand_enum(AI::Weapon::SPANNER);
        AI::Card weapon(w);
        REQUIRE(weapon.type == AI::Card::Type::WEAPON);
        REQUIRE(AI::Weapon(weapon.card) == w);

        // test room
        auto r = rand_enum(AI::Room::GAMES_ROOM);
        AI::Card room(r);
        REQUIRE(room.type == AI::Card::Type::ROOM);
        REQUIRE(AI::Room(room.card) == r);
    }

    SECTION("suggestion") {
        auto p = rand_enum(AI::Player::WHITE);
        auto w = rand_enum(AI::Weapon::SPANNER);
        auto r = rand_enum(AI::Room::GAMES_ROOM);
        AI::Suggestion sug(p, w, r);
        REQUIRE(sug.player == p);
        REQUIRE(sug.weapon == w);
        REQUIRE(sug.room == r);
    }
}

// vim: set expandtab textwidth=100:
