#include <catch/catch.hpp>
#include "../include/position.h"
#include "../include/board.h"

using namespace AI;
using Catch::Matchers::Equals;

TEST_CASE("Position class", "[position]") {
    SECTION("init") {
        REQUIRE_THAT(Position(0).getNeighbours(), Equals(Board::board[0]));
        REQUIRE_THAT(Position(20).getNeighbours(), Equals(Board::board[20]));
        REQUIRE_THAT(Position(50).getNeighbours(), Equals(Board::board[50]));
    }

    SECTION("basic distance logic") {
        // same position
        REQUIRE(Position(0).dist(0) == 0);
        // basic floor path logic
        REQUIRE(int(Position(15).dist(24)) == 9);
        REQUIRE(int(Position(11).dist(12)) == 8);
        REQUIRE(int(Position(42).dist(43)) == 7);
        // room neighbour floor tile
        REQUIRE(int(Position(1).dist(40)) == 1);
        // enter room
        REQUIRE(int(Position(40).dist(1)) == 1);
        // check that passing through a room doesn't work (doesn't check for correct value, just checks
        // that it doesn't shortcut through rooms)
        REQUIRE(int(Position(36).dist(37)) > 2);
        REQUIRE(int(Position(7).dist(82)) > 2);
        REQUIRE(int(Position(8).dist(10)) > 2);
        // check that passing from one room to another doesn't take a dice roll (using either secret
        // passage or if the rooms are connected)
        REQUIRE(int(Position(7).dist(8)) == 0);
        REQUIRE(int(Position(2).dist(7)) == 0);
        REQUIRE(int(Position(4).dist(5)) == 0);
    }

    SECTION("multiple turns logic") {
        // test if going through rooms works if multiple turns can be used
        REQUIRE(int(Position(7).dist(82, 2)) == 1);
        REQUIRE(int(Position(3).dist(10, 3)) == 8);

        // test that the middle room is never used as a shortcut
        REQUIRE(int(Position(36).dist(37, 2)) > 2);
    }

    SECTION("special middle room logic") {
        // allows going out of the room
        REQUIRE(int(Position(0).dist(20)) == 1);
        // going into the room works as well
        REQUIRE(int(Position(20).dist(0)) == 1);
    }
}

// vim: set expandtab textwidth=100:
