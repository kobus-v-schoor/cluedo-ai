/**
 * \file no-show.cpp
 * \author Kobus van Schoor
 */

#include "../../include/predictors/no-show.h"

using namespace AI;

void NoShowPredictor::run(Deck& deck, std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>>
        notes, Bot::SuggestionLog log)
{
    for (auto l : log.log()) {
        if (l.showed)
            continue;

        Bot::Player from = l.from;

        Bot::Player p = l.suggestion.player;
        Bot::Weapon w = l.suggestion.weapon;
        Bot::Room r = l.suggestion.room;

        if (notes[from][p].has && notes[from][w].has && deck.contains(r)) {
            LOG_LOGIC("Identified " + Bot::enumToStr(r) + " as a very high probability card");
            deck.scores[r] -= 5;
        }

        if (notes[from][p].has && notes[from][r].has && deck.contains(w)) {
            LOG_LOGIC("Identified " + Bot::enumToStr(w) + " as a very high probability card");
            deck.scores[w] -= 5;
        }

        if (notes[from][w].has && notes[from][r].has && deck.contains(p)) {
            LOG_LOGIC("Identified " + Bot::enumToStr(p) + " as a very high probability card");
            deck.scores[p] -= 5;
        }
    }
}

// vim: set expandtab textwidth=100:
