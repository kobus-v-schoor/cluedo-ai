/**
 * \file multiple.cpp
 * \author Kobus van Schoor
 */

#include "../../include/predictors/multiple.h"

using namespace AI;

MultiplePredictor::MultiplePredictor(Bot::Player player)
{
    this->player = player;
}

void MultiplePredictor::run(Deck& deck, std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>>
        notes, Bot::SuggestionLog log)
{
    std::map<Bot::Player, std::map<Bot::Card, int>> sc;
    for (auto l : log.log()) {
        Bot::Suggestion sug = l.suggestion;
        Bot::Player player = l.from;

        std::vector<Bot::Card> cards;
        if (deck.contains(sug.player))
            cards.push_back(sug.player);
        if (deck.contains(sug.weapon))
            cards.push_back(sug.weapon);
        if (deck.contains(sug.room))
            cards.push_back(sug.room);

        for (auto card : cards)
            if (notes[player][card].lacks)
                sc[player][card]++;
    }

    for (auto player : sc) {
        for (auto card : player.second) {
            if (card.second > 1) {
                LOG_LOGIC("Identified " + std::string(card.first) + " as a high probability card");
                deck.scores[card.first]--;
            }
        }
    }
}

// vim: set expandtab textwidth=100:
