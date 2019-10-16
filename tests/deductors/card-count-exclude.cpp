#include <catch/catch.hpp>
#include "../../include/deductors/card-count-exclude.h"

using namespace AI;

template <typename T>
void erase(std::vector<T>& vec, T obj)
{
    vec.erase(std::find(vec.begin(), vec.end(), obj));
}

TEST_CASE("CardCountExclude", "[card-count-exclude-deductor]") {
    std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>> notes;
    std::vector<Bot::Player> order = { Bot::SCARLET, Bot::PLUM, Bot::PEACOCK, Bot::GREEN };
    if (rand() % 2)
        order.push_back(Bot::MUSTARD);

    Bot::Player player = order[rand() % order.size()];
    Bot::SuggestionLog log;
    const unsigned int totalCards = 18;
    const unsigned int cardsPerPlayer = (totalCards - (totalCards % order.size())) / order.size();
    const unsigned int tableCardsCount = totalCards % order.size();

    REQUIRE(((order.size() == 4) || (order.size() == 5))); // to ensure that there are table cards

    std::vector<Bot::Card> cards;

    for (int i = 0; i <= int(Bot::MAX_PLAYER); i++)
        cards.push_back(Bot::Player(i));
    for (int i = 0; i <= int(Bot::MAX_WEAPON); i++)
        cards.push_back(Bot::Weapon(i));
    for (int i = 0; i <= int(Bot::MAX_ROOM); i++)
        cards.push_back(Bot::Room(i));

    std::vector<Bot::Card> tableCards;

    while (tableCards.size() < tableCardsCount) {
        Bot::Card card = cards[rand() % cards.size()];
        erase(cards, card);
        tableCards.push_back(card);
    }

    std::vector<Bot::Card> deck;

    while (deck.size() < cardsPerPlayer) {
        Bot::Card card = cards[rand() % cards.size()];
        erase(cards, card);
        deck.push_back(card);
    }

    REQUIRE(deck.size() > 0);

    CardCountExcludeDeductor deductor(player, order);

    REQUIRE_FALSE(deductor.run(log, notes));

    SECTION("ignore table cards") {
        for (auto t : tableCards) {
            notes[player][t].has = true;
            notes[player][t].table = true;
        }

        for (size_t i = 0; i < cardsPerPlayer - tableCardsCount; i++)
            notes[player][deck[i]].has = true;

        REQUIRE_FALSE(deductor.run(log, notes));
        for (auto t : cards)
            REQUIRE_FALSE(notes[player][t].lacks);
    }

    SECTION("almost reached") {
        for (size_t i = 0; i < deck.size() - 1; i++)
            notes[player][deck[i]].has = true;

        REQUIRE_FALSE(deductor.run(log, notes));
        for (auto t : cards)
            REQUIRE_FALSE(notes[player][t].lacks);
    }

    SECTION("reached") {
        for (size_t i = 0; i < deck.size(); i++)
            notes[player][deck[i]].has = true;

        REQUIRE(deductor.run(log, notes));
        for (auto t : cards)
            REQUIRE(notes[player][t].lacks);
        REQUIRE_FALSE(deductor.run(log, notes));
    }
}

// vim: set expandtab textwidth=100:
