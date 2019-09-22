/**
 * \file board.h
 * \author Kobus van Schoor
 */

#pragma once
#include <vector>

namespace AI {
    /**
     * \brief Groups the board functionality
     *
     * This namespace will group all the functions and variables needed to represent the board as a
     * graph and will be used to do path calculations. The board graph will exist as a single
     * external global variable. This is done because the board will frequently be re-used and won't
     * be modified or change based on the AI using it.
     */
    namespace Board {
        /**
         * \brief Total amount of positions on the board
         * \note Maximum allowable position is BOARD_SIZE - 1 since positions start at zero
         */
        const int BOARD_SIZE = 83;

        /**
         * \brief Amount of rooms on the board i.e. all positions smaller than this number is a room
         */
        const int ROOM_COUNT = 10;

        /**
         * Every position on the board will be represented by exactly one node, irrespective if it
         * is a room or a cell on the floor grid. Connections (and hence valid movement paths) are
         * represented by edges. Nodes are the vectors in the board vector, and edges are the
         * integers within the board vectors representing the indices of the nodes they are
         * connected to.
         */
        extern std::vector<std::vector<int>> board;
    };
}

// vim: set expandtab textwidth=100:
