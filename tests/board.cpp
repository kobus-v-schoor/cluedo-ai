#include <catch/catch.hpp>
#include <algorithm>
#include "../include/board.h"

using namespace AI::Board;
using Catch::Matchers::VectorContains;

bool contains(std::vector<int> vec, int v) {
    return std::find(vec.begin(), vec.end(), v) != vec.end();
}

TEST_CASE("board namespace", "[board]") {
    // check that board was actually initialized
    REQUIRE(board.size() == 83);

    // basic
    REQUIRE_THAT(board[10], VectorContains(11));

    // check for dual-edge connection
    REQUIRE_THAT(board[11], VectorContains(10));

    // some corner-cases (pun intended)
    REQUIRE_FALSE(contains(board[42], 43));
    REQUIRE_FALSE(contains(board[43], 53));

    // some room entrances
    REQUIRE_THAT(board[0], VectorContains(36));
    REQUIRE_THAT(board[7], VectorContains(8));
    REQUIRE_THAT(board[9], VectorContains(13));

    // secret passages
    REQUIRE_THAT(board[7], VectorContains(2));
    REQUIRE_THAT(board[4], VectorContains(9));
}

// vim: set expandtab textwidth=100:
