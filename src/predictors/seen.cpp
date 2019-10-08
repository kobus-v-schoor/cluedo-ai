/**
 * \file seen.cpp
 * \author Kobus van Schoor
 */

#include "../../include/predictors/seen.h"

using namespace AI;

void SeenPredictor::run(Deck& deck, std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>> notes,
        Bot::SuggestionLog log)
{
    for (auto l : log.log()) {
        if (!l.showed)
            continue;

        Bot::Suggestion sug = l.suggestion;

        if (notes[l.from][sug.player].seen && notes[l.from][sug.weapon].seen &&
                !notes[l.from][sug.room].seen && contains(deck, sug.room)) {
            LOG_LOGIC("Identified " + Bot::roomToStr(sug.room) + " as a low probability card");
            deck.scores[sug.room]++;
        } else if (notes[l.from][sug.player].seen && notes[l.from][sug.room].seen &&
                !notes[l.from][sug.weapon].seen && contains(deck, sug.weapon)) {
            LOG_LOGIC("Identified " + Bot::weaponToStr(sug.weapon) + " as a low probability card");
            deck.scores[sug.weapon]++;
        } else if (notes[l.from][sug.weapon].seen && notes[l.from][sug.room].seen &&
                !notes[l.from][sug.player].seen && contains(deck, sug.player)) {
            LOG_LOGIC("Identified " + Bot::playerToStr(sug.player) + " as a low probability card");
            deck.scores[sug.player]++;
        }
    }
}

// vim: set expandtab textwidth=100:
