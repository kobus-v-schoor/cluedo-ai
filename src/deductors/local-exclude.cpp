/**
 * \file local-exclude.cpp
 * \author Kobus van Schoor
 */

#include "../../include/deductors/local-exclude.h"

using namespace AI;

bool LocalExcludeDeductor::run(Bot::SuggestionLog log, std::map<Bot::Player,
        std::map<Bot::Card, Bot::Notes>>& notes)
{
    auto logi = log.log();
    bool found = false;

    for (auto l : logi) {
        if (!l.showed)
            continue;

        Bot::Suggestion sug = l.suggestion;

        if ((notes[l.show][sug.player].lacks && notes[l.show][sug.weapon].lacks) &&
                !notes[l.show][sug.room].concluded()) {
            found = true;
            LOG_LOGIC("Deduced that " << l.show << " has " << sug.room << " (suggestion:" << sug
                    << ") (local-exclude)");
            notes[l.show][sug.room].has = true;
            notes[l.show][sug.room].deduced = true;
        } else if ((notes[l.show][sug.player].lacks && notes[l.show][sug.room].lacks) &&
                !notes[l.show][sug.weapon].concluded()) {
            found = true;
            LOG_LOGIC("Deduced that " << l.show << " has " << sug.weapon << " (suggestion:" << sug
                    << ") (local-exclude)");
            notes[l.show][sug.weapon].has = true;
            notes[l.show][sug.weapon].deduced = true;
        } else if ((notes[l.show][sug.weapon].lacks && notes[l.show][sug.room].lacks) &&
                !notes[l.show][sug.player].concluded()) {
            found = true;
            LOG_LOGIC("Deduced that " << l.show << " has " << sug.player << " (suggestion:" << sug
                    << ") (local-exclude)");
            notes[l.show][sug.player].has = true;
            notes[l.show][sug.player].deduced = true;
        }
    }

    return found;
}

// vim: set expandtab textwidth=100:
