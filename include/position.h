#pragma once
#include "macros.h"
#include <vector>

namespace AI {
    /**
     * \brief Abstraction of player positions
     *
     * This will be used whenever a positions needs to be specified. This will also be used for
     * calculations of path distances.
     */
    class Position {
        public:
            /**
             * \brief Contains a path of positions and the distance to travel the path
             */
            class Path {
                public:
                    Path(int start_pos);
                    void append(int pos);
                    void append(Path other);
                    std::vector<int> getPath();

                    operator int() const;
                    bool operator<(const Path& other);
                private:
                    std::vector<int> path;
            };

            /**
             * \param pos This is the integer position used in the board layout
             * \throw std::invalid_argument if position is not a valid board position
             */
            Position(int pos);

            /**
             * \brief Returns the valid neighbours of the current position
             * \returns position's valid neighbours
             */
            std::vector<int> getNeighbours();

            /**
             * \brief Returns the distance from another position
             *
             * This returns the distance from this board position to another position. This distance
             * represents the count for the dice roll needed to reach the other position. The
             * distance between two positions are the same regardless of the starting point. A turns
             * parameter can also be passed to check the distance if more than 1 turn can be used.
             *
             * \param other the other Position
             * \param turns the amount of turns that can be used to reach position
             * \returns a Path that contains the distance and the path that was followed
             */
            Path dist(const Position other, int turns=1);

        private:
            void _dist(int start, int dest, std::vector<bool>& visited, std::vector<Path>& sps, int turns);

            const int position;
    };
}

// vim: set expandtab textwidth=100:
