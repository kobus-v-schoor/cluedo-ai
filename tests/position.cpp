#include <catch/catch.hpp>
#include <stdexcept>
#include "../include/position.h"
#include "../include/board.h"

using namespace AI;
using Catch::Matchers::Equals;
using Catch::Matchers::VectorContains;

TEST_CASE("Path struct", "[path]") {
    Position::Path p(25);

    // empty
    REQUIRE(int(p) == 0);

    // one tile
    p.append(15);
    REQUIRE(int(p) == 1);

    // two tiles
    p.append(10);
    REQUIRE(int(p) == 2);

    // into room
    p.append(7);
    REQUIRE(int(p) == 3);

    // through room - shouldn't add dist
    p.append(8);
    REQUIRE(int(p) == 3);
}

void validatePath(Position::Path path)
{
    auto p = path.getPath();
    for (size_t i = 0; i < p.size() - 1; i++)
        REQUIRE_THAT(Board::board[p[i]], VectorContains(p[i+1]));
}

TEST_CASE("Position class", "[position]") {
    SECTION("init") {
        REQUIRE_NOTHROW(Position(0));
        REQUIRE_NOTHROW(Position(40));
        REQUIRE_NOTHROW(Position(82));

        REQUIRE_THROWS_AS(Position(-1), std::invalid_argument&);
        REQUIRE_THROWS_AS(Position(83), std::invalid_argument&);

        REQUIRE_THAT(Position(0).getNeighbours(), Equals(Board::board[0]));
        REQUIRE_THAT(Position(20).getNeighbours(), Equals(Board::board[20]));
        REQUIRE_THAT(Position(50).getNeighbours(), Equals(Board::board[50]));
    }

    SECTION("basic distance logic") {
        SECTION("nearby or trivial paths") {
            REQUIRE(Position(0).path(0) == 0);
            REQUIRE(int(Position(10).path(11)) == 1);
            REQUIRE(int(Position(10).path(16)) == 2);
            REQUIRE(int(Position(10).path(17)) == 3);
            REQUIRE(int(Position(10).path(18)) == 4);
            REQUIRE(int(Position(10).path(27)) == 4);
            REQUIRE(int(Position(15).path(24)) == 9);
        }

        SECTION("corner logic") {
            REQUIRE(int(Position(11).path(12)) == 8);
            REQUIRE(int(Position(42).path(43)) == 7);
            REQUIRE(int(Position(26).path(30)) == 9);
        }

        SECTION("long path") {
            REQUIRE(int(Position(10).path(82)) == 16);
            // reverse of long path
            REQUIRE(int(Position(82).path(10)) == 16);
        }

        SECTION("enter and exit room") {
            // exit room
            REQUIRE(int(Position(1).path(40)) == 1);
            // enter room
            REQUIRE(int(Position(40).path(1)) == 1);
        }

        SECTION("no shortcuts through rooms") {
            REQUIRE(int(Position(36).path(37)) > 2);
            REQUIRE(int(Position(7).path(82)) > 2);
            REQUIRE(int(Position(8).path(10)) > 2);
            REQUIRE(int(Position(7).path(19)) > 1);
        }

        SECTION("direct room to room distance") {
            REQUIRE(int(Position(7).path(8)) == 0);
            REQUIRE(int(Position(2).path(7)) == 0);
            REQUIRE(int(Position(4).path(5)) == 0);
        }

        SECTION("validate paths") {
            validatePath(Position(10).path(11));
            validatePath(Position(10).path(16));
            validatePath(Position(42).path(43));
            validatePath(Position(10).path(82));
            validatePath(Position(82).path(10));
            validatePath(Position(10).path(82, 3));
        }
    }

    // test if going through rooms works if multiple turns can be used
    SECTION("multiple turns logic") {
        REQUIRE(int(Position(7).path(82, 2)) == 1);
        REQUIRE(int(Position(7).path(19, 2)) == 1);
        REQUIRE(int(Position(3).path(10, 3)) == 7);
    }

    SECTION("special middle room logic") {
        // never use as shortcut (even with multiple turns)
        REQUIRE(int(Position(36).path(37, 2)) > 2);
        // exit room
        REQUIRE(int(Position(0).path(20)) == 1);
        // enter room
        REQUIRE(int(Position(20).path(0)) == 1);
    }
}

// vim: set expandtab textwidth=100:
