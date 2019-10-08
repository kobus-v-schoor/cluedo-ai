/**
 * \file card-count-exclude.cpp
 * \author Kobus van Schoor
 */

#include "../../include/deductors/card-count-exclude.h"

using namespace AI;

std::vector<Bot::Card> CardCountExcludeDeductor::allCards;

CardCountExcludeDeductor::CardCountExcludeDeductor(std::vector<Bot::Player> order) :
    order(order)
{
    if (allCards.empty()) {
        for (int i = 0; i <= int(Bot::MAX_PLAYER); i++)
            allCards.push_back(Bot::Player(i));
        for (int i = 0; i <= int(Bot::MAX_WEAPON); i++)
            allCards.push_back(Bot::Weapon(i));
        for (int i = 0; i <= int(Bot::MAX_ROOM); i++)
            allCards.push_back(Bot::Room(i));
    }
}

bool CardCountExcludeDeductor::run(Bot::SuggestionLog log, std::map<Bot::Player, std::map<Bot::Card,
        Bot::Notes>>& notes)
{
    const int totalCards = 18; // 21 total - 3 in envelope
    const int cardsPerPlayer = (totalCards - (totalCards % order.size())) / order.size();
    bool found = false;

    for (auto player : order) {
        int cc = 0;
        for (auto c : notes[player])
            if (c.second.has && !c.second.table)
                cc++;

        if (cc >= cardsPerPlayer) {
            for (auto c : allCards) {
                if (!notes[player][c].has && !notes[player][c].lacks) {
                    LOG_LOGIC("Deduced that " + Bot::playerToStr(player) + " lacks " +
                            std::string(c) + " (card-count-exclude)");
                    notes[player][c].lacks = true;
                    found = true;
                }
            }
        }
    }

    return found;
}

// vim: set expandtab textwidth=100:
