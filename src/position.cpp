#include "../include/position.h"
#include "../include/board.h"
#include <stdexcept>

using namespace AI;

Position::Path::operator int()
{
    return dist;
}

Position::Position(int pos) :
    position(pos)
{
    if ((pos < 0) || (pos > 82))
        throw std::invalid_argument("pos must be between 0 and 82, inclusive");
}

std::vector<int> Position::getNeighbours()
{
    return Board::board[this->position];
}

Position::Path Position::dist(const Position other, int turns) { return Path(); }

// vim: set expandtab textwidth=100:
