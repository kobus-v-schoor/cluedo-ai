#pragma once
#include "macros.h"

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
             * \param pos This is the integer position used in the board layout
             */
            NOT_IMPLEMENTED
            Position(int pos);
    };
}

// vim: set expandtab textwidth=100:
