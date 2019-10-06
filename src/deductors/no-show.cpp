/**
 * \file no-show.cpp
 * \author Kobus van Schoor
 */

#include "../../include/deductors/no-show.h"

using namespace AI;

NoShowDeductor::NoShowDeductor(std::vector<Bot::Player> order) :
    order(order)
{}

bool NoShowDeductor::run(Bot::SuggestionLog log, std::map<Bot::Player,
        std::map<Bot::Card, Bot::Notes>>& notes)
{
    bool found = false;

    for (auto l : log.log()) {
        size_t start = 0;
        while ((order[start] != l.from) && (start < order.size()))
            start++;

        // not doing anything about this error since it is unlikely to occur and non-fatal. If it
        // does occur it will alert the user but not much else.
        if (start >= order.size()) {
            LOG_ERR("ran out of players in order - order array is probably wrong");
            continue;
        }

        size_t end = start + 1;

        if (l.showed) {
            while (end != start) {
                if (end >= order.size())
                    end = 0;
                if (order[end] == l.show)
                    break;
                end++;
            }
        } else
            end = start;

        // not doing anything about this error since it is unlikely to occur and non-fatal. If it
        // does occur it will alert the user but not much else.
        if ((end == start) && l.showed) {
            LOG_ERR("player not found in order - order array is probably wrong");
            continue;
        }

        size_t pos = ((start + 1) == order.size()) ? 0 : start + 1;
        while (pos != end) {
            for (auto c : { Bot::Card(l.suggestion.player), Bot::Card(l.suggestion.weapon),
                    Bot::Card(l.suggestion.room) }) {
                if (!notes[order[pos]][c].concluded()) {
                    found = true;
                    LOG_LOGIC("Deduced that " << order[pos] << " lacks " << c << " (no-show)");
                    notes[order[pos]][c].lacks = true;
                }
            }

            pos++;
            if (pos >= order.size())
                pos = 0;
        }
    }

    return found;
}

// vim: set expandtab textwidth=100:
