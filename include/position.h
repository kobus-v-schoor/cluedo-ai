/**
 * \file position.h
 * \author Kobus van Schoor
 */

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
             *
             * This class can be used to represent a path followed on the board and also to get the
             * dice roll count required to complete the path
             */
            class Path {
                public:
                    /**
                     * \param start_pos the position where the path should start
                     */
                    Path(int start_pos);

                    /**
                     * \brief Returns true if path is empty
                     *
                     * A path is considered empty if the only element in the path is the starting
                     * point
                     *
                     * \returns true if the path is empty
                     */
                    bool empty();

                    /**
                     * \brief Appends the position to the path (i.e. appends to the end of the path)
                     * \param pos the position to append
                     */
                    void append(int pos);

                    /**
                     * \brief Appends another path to the end of the current path
                     * \param other the other path
                     */
                    void append(Path other);

                    /**
                     * \brief Returns the path in the form of a vector
                     * \returns the path
                     */
                    std::vector<int> getPath() const;

                    /**
                     * \brief Returns the distance (i.e. the dice roll count required) for the path
                     * \returns the distance
                     */
                    int dist() const;

                    /**
                     * \brief Returns up to where in the path you can get in the moves given
                     *
                     * This assumes that the path must be followed in one turn only. So, if a room
                     * is encountered along the path, the path stops there.
                     *
                     * \param moves the amount of moves allowed
                     * \returns the position in the path where you can get with the moves given
                     */
                    int partial(int moves);

                    /**
                     * \brief Alias for dist()
                     */
                    operator int() const;

                    /**
                     * \brief Can be used to compare the distance of two paths
                     *
                     * If one path is less than another, it means that the path is shorter than the
                     * other path.
                     *
                     * \param other the other path
                     * \returns true if the path is shorter than the other path
                     */
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
             * \brief Returns the shortest path to another position
             *
             * This returns the shortest path from this board position to another position. By
             * making turns more than one, shortcuts through rooms can also be used
             *
             * \param other the other Position
             * \param occupied positions on the board occupied by other players
             * \param turns the amount of turns that can be used to reach position
             * \returns a Path class that contains the distance and the path that was followed
             * \throw std::invalid_argument if occupied isn't the correct size or if turns is less
             * than 1
             * \throw std::runtime_error if no path can be found to destination - this should only
             * happen when a tile is blocked because all the neighbours to the tile is occupied
             */
            Path path(const Position other, std::vector<bool> occupied, int turns);

            /**
             * \brief Overload for path() with occupied all false and turns = 1
             * \note This class is intended to ease unit testing.
             */
            Path path(const Position other);

            /**
             * \brief Overload for path() with occupied all false
             * \note This class is intended to ease unit testing.
             */
            Path path(const Position other, int turns);

            /**
             * \brief Overload for path() with turns = 1
             * \note This class is intended to ease unit testing.
             */
            Path path(const Position other, std::vector<bool> occupied);

            /**
             * \brief Allows casting the position to an int where the int is the position on the
             * board
             */
            operator int() const;

        private:
            /**
             * \brief Used to passed along info from one recursion to the next of the shortestPath()
             * fuction
             */
            struct SPInfo {
                int start;
                int dest;
                std::vector<bool> occupied;
                std::vector<bool> visited;
                /**
                 * \brief Will be updated during execution to always have to shortest path to a node
                 * from the starting point
                 */
                std::vector<Path> spMap;
            };

            void shortestPath(SPInfo& info, int turns);

            const int position;
    };
}

// vim: set expandtab textwidth=100:
