#include <catch/catch.hpp>
#include <stdexcept>
#include "../include/position.h"
#include "../include/board.h"

using namespace AI;
using Catch::Matchers::Equals;
using Catch::Matchers::VectorContains;

TEST_CASE("Path struct", "[path]") {
    SECTION("dist") {
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

    SECTION("partial") {
        SECTION("just tiles") {
            Position::Path path(54);
            path.append(64);
            path.append(65);
            path.append(66);
            path.append(56);
            path.append(46);
            path.append(36);
            path.append(0);

            REQUIRE_THAT(path.getPath(), Equals(std::vector<int>({54, 64, 65, 66, 56, 46, 36, 0})));

            REQUIRE(path.partial(0) == 54);
            REQUIRE(path.partial(1) == 64);
            REQUIRE(path.partial(2) == 65);
            REQUIRE(path.partial(7) == 0);
            REQUIRE(path.partial(8) == 0);
        }

        SECTION("through room") {
            Position::Path path(10);
            path.append(7);
            path.append(8);
            path.append(19);

            REQUIRE(path.partial(4) == 7);
        }
    }
}

void validatePath(Position::Path path, std::vector<bool> occupied = std::vector<bool>())
{
    auto p = path.getPath();
    for (size_t i = 0; i < p.size() - 1; i++) {
        REQUIRE_THAT(Board::board[p[i]], VectorContains(p[i+1]));
        if (!occupied.empty())
            REQUIRE_FALSE(occupied[p[i+1]]);
    }
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

        REQUIRE(int(Position(10)) == 10);

        Position::Path p1(0);
        p1.append(1);
        p1.append(2);

        Position::Path p2(3);
        p2.append(4);
        p2.append(5);

        p1.append(p2);

        REQUIRE_THAT(p1.getPath(), Equals(std::vector<int>{0, 1, 2, 3, 4, 5}));
    }

    SECTION("path logic") {
        SECTION("argument checks") {
            REQUIRE_NOTHROW(Position(10).path(11, std::vector<bool>(Board::BOARD_SIZE)));
            REQUIRE_NOTHROW(Position(10).path(11, 1));
            REQUIRE_NOTHROW(Position(10).path(11, 2));

            REQUIRE_THROWS_AS(Position(10).path(11, std::vector<bool>()), std::invalid_argument&);
            REQUIRE_THROWS_AS(Position(10).path(11, std::vector<bool>(10)), std::invalid_argument&);
            REQUIRE_THROWS_AS(Position(10).path(11, 0), std::invalid_argument&);
            REQUIRE_THROWS_AS(Position(10).path(11, -1), std::invalid_argument&);
        }

        SECTION("nearby or trivial paths") {
            REQUIRE(Position(0).path(0) == 0);
            REQUIRE(int(Position(10).path(11)) == 1);
            REQUIRE(int(Position(10).path(16)) == 2);
            REQUIRE(int(Position(10).path(17)) == 3);
            REQUIRE(int(Position(10).path(18)) == 4);
            REQUIRE(int(Position(10).path(27)) == 4);
            REQUIRE(int(Position(15).path(24)) == 9);
        }

        SECTION("corners") {
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

        // test if going through rooms works if multiple turns can be used
        SECTION("multiple turns") {
            REQUIRE(int(Position(7).path(82, 2)) == 1);
            REQUIRE(int(Position(7).path(19, 2)) == 1);
            REQUIRE(int(Position(3).path(10, 3)) == 7);
        }

        SECTION("middle room") {
            // never use as shortcut (even with multiple turns)
            REQUIRE(int(Position(36).path(37, 2)) > 2);
            // exit room
            REQUIRE(int(Position(0).path(20)) == 1);
            // enter room
            REQUIRE(int(Position(20).path(0)) == 1);
        }

        SECTION("occupied tiles") {
            std::vector<bool> occupied(Board::BOARD_SIZE, false);

            SECTION("go around occupied tile") {
                occupied[26] = true;
                // go around occupied tile
                REQUIRE(int(Position(25).path(27, occupied)) == 4);
                validatePath(Position(25).path(27, occupied), occupied);
            }

            SECTION("check blocked off access") {
                occupied[33] = true;
                occupied[44] = true;
                REQUIRE_THROWS_AS(Position(43).path(34, occupied), std::runtime_error&);
            }

            SECTION("occupied rooms") {
                occupied[7] = true;
                REQUIRE(int(Position(10).path(8, occupied, 2)) == 1);
            }
        }
    }
}

// vim: set expandtab textwidth=100:
