#include "../include/position.h"
#include "../include/board.h"

using namespace AI;

Position::Path::operator int()
{
    return dist;
}

Position::Position(int pos) :
    position(pos)
{}

std::vector<int> Position::getNeighbours()
{
    return Board::board[this->position];
}

Position::Path Position::dist(const Position other, int turns) { return Path(); }

// vim: set expandtab textwidth=100:
