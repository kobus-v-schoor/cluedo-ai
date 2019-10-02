/**
 * \file position.cpp
 * \author Kobus van Schoor
 */

#include "../include/position.h"
#include "../include/board.h"
#include <stdexcept>

using namespace AI;

Position::Path::Path(int start_pos)
{
    path.push_back(start_pos);
}

bool Position::Path::empty()
{
    return path.size() == 1;
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
        if (!((path[i-1] < Board::ROOM_COUNT) && (path[i] < Board::ROOM_COUNT)))
            d++;
    return d;
}

int Position::Path::partial(int moves)
{
    Path par(path[0]);
    size_t pos = 1;

    while ((pos < path.size()) && (par.dist() < moves) && (par.path.back() >= Board::ROOM_COUNT))
        par.path.push_back(path[pos++]);

    return par.path.back();
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
    if ((pos < 0) || (pos >= Board::BOARD_SIZE))
        throw std::invalid_argument("pos must be between 0 and " +
                std::to_string(Board::BOARD_SIZE-1) + ", inclusive");
}

std::vector<int> Position::getNeighbours()
{
    return Board::board[this->position];
}

Position::Path Position::path(const Position other, std::vector<bool> occupied, int turns)
{
    if (occupied.size() != Board::BOARD_SIZE)
        throw std::invalid_argument("occupied vector must be the size of the board");
    if (turns < 1)
        throw std::invalid_argument("turns must be at least 1");

    // same position
    if (other.position == position)
        return Path(position);

    SPInfo info;

    info.start = position;
    info.dest = other.position;
    info.occupied = occupied;
    info.visited = std::vector<bool>(Board::BOARD_SIZE, false);
    info.spMap = std::vector<Path>(Board::BOARD_SIZE, position);

    shortestPath(info, turns);

    if (info.spMap[info.dest].empty())
        throw std::runtime_error("unable to find valid path from " + std::to_string(position) +
                " to " + std::to_string(other.position));
    return info.spMap[info.dest];
}

Position::Path Position::path(const Position other)
{
    return path(other, std::vector<bool>(Board::BOARD_SIZE, false), 1);
}

Position::Path Position::path(const Position other, int turns)
{
    return path(other, std::vector<bool>(Board::BOARD_SIZE, false), turns);
}

Position::Path Position::path(const Position other, std::vector<bool> occupied)
{
    return path(other, occupied, 1);
}

Position::operator int() const
{
    return position;
}

void Position::shortestPath(SPInfo& info, int turns)
{
    info.visited[position] = true;

    for (auto ngh : getNeighbours()) {
        // create path from this node to neigbour
        Path p(info.spMap[position]);
        p.append(ngh);

        // check if newly created path is shorter than the current one
        if (((info.spMap[ngh].empty()) || (p < info.spMap[ngh])) && (ngh != info.start)) {
            info.visited[ngh] = false;
            info.spMap[ngh] = p;
        }

        // skip occupied tile if it's not a room
        if (info.occupied[ngh] && (ngh >= Board::ROOM_COUNT))
            continue;

        // destination found, return
        if (ngh == info.dest)
            return;

        // skip visited
        if (info.visited[ngh])
            continue;

        // middle room
        if (ngh == 0)
            continue;

        // room and only one turn
        if ((ngh < Board::ROOM_COUNT) && (turns <= 1))
            continue;

        Position(ngh).shortestPath(info, ngh < Board::ROOM_COUNT ? turns - 1 : turns);
    }
}

// vim: set expandtab textwidth=100:
