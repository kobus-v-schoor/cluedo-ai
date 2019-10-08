/**
 * \file seen.cpp
 * \author Kobus van Schoor
 */

#include "../../include/deductors/seen.h"

using namespace AI;

bool SeenDeductor::run(Bot::SuggestionLog log, std::map<Bot::Player, std::map<Bot::Card,
        Bot::Notes>>& notes)
{
    bool found = false;

    for (auto l : log.log()) {
        if (!l.showed)
            continue;

        Bot::Suggestion sug = l.suggestion;

        if (notes[l.show][sug.player].lacks && notes[l.show][sug.weapon].lacks &&
                !notes[l.from][sug.room].seen) {
            found = true;
            LOG_LOGIC("Deduced that " + Bot::playerToStr(l.from) + " saw " +
                    Bot::roomToStr(sug.room) + " (seen)");
            notes[l.from][sug.room].seen = true;
        } else if (notes[l.show][sug.player].lacks && notes[l.show][sug.room].lacks &&
                !notes[l.from][sug.weapon].seen) {
            found = true;
            LOG_LOGIC("Deduced that " + Bot::playerToStr(l.from) + " saw " +
                    Bot::weaponToStr(sug.weapon) + " (seen)");
            notes[l.from][sug.weapon].seen = true;
        } else if (notes[l.show][sug.weapon].lacks && notes[l.show][sug.room].lacks &&
                !notes[l.from][sug.player].seen) {
            found = true;
            LOG_LOGIC("Deduced that " + Bot::playerToStr(l.from) + " saw " +
                    Bot::playerToStr(sug.player) + " (seen)");
            notes[l.from][sug.player].seen = true;
        }
    }

    return found;
}

// vim: set expandtab textwidth=100:
