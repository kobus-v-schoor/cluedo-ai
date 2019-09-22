#include "../include/position.h"
#include "../include/board.h"
#include <stdexcept>

using namespace AI;

Position::Path::Path(int start_pos)
{
    path.push_back(start_pos);
}

void Position::Path::append(int pos)
{
    path.push_back(pos);
}

void Position::Path::append(Path other)
{
    path.insert(path.end(), other.path.begin(), other.path.end());
}

std::vector<int> Position::Path::getPath() const
{
    return path;
}

int Position::Path::dist() const
{
    int d = 0;
    for (size_t i = 1; i < path.size(); i++)
        if (!((path[i-1] < 10) && (path[i] < 10)))
            d++;
    return d;
}

Position::Path::operator int() const
{
    return dist();
}

bool Position::Path::operator<(const Path& other)
{
    return int(*this) < int(other);
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

Position::Path Position::path(const Position other, int turns)
{
    // same position
    if (other.position == position)
        return Path(position);

    std::vector<bool> visited(Board::board.size(), false);
    std::vector<Path> sps(Board::board.size(), position);

    _dist(position, other.position, visited, sps, turns);
    return sps[other.position];
}

void Position::_dist(int start, int dest, std::vector<bool>& visited, std::vector<Path>& sps, int turns)
{
    visited[position] = true;

    for (auto ngh : getNeighbours()) {
        Path p(sps[position]);
        p.append(ngh);
        if (((sps[ngh] == 0) || (p < sps[ngh])) && (ngh != start)) {
            visited[ngh] = false;
            sps[ngh] = p;
        }

        if (ngh == dest)
            return;

        // skip visited
        if (visited[ngh])
            continue;

        // middle room
        if (ngh == 0)
            continue;

        // room and only one turn
        if ((ngh < 10) && (turns <= 1))
            continue;

        Position(ngh)._dist(start, dest, visited, sps, ngh < 10 ? turns - 1 : turns);
    }
}

// vim: set expandtab textwidth=100:
