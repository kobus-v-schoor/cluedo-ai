#include <catch/catch.hpp>
#include <vector>
#include <algorithm>
#include <iostream>
#include "../include/bot.h"
#include "../include/board.h"

using namespace AI;

template <typename T>
void erase(std::vector<T>& vec, T obj)
{
    vec.erase(std::find(vec.begin(), vec.end(), obj));
}

template <typename T>
bool contains(std::vector<T> vec, T obj)
{
    return std::find(vec.begin(), vec.end(), obj) != vec.end();
}

int getRoomPos(Bot::Room room)
{
    switch (room) {
        case Bot::BEDROOM: return 4;
        case Bot::BATHROOM: return 5;
        case Bot::STUDY: return 6;
        case Bot::KITCHEN: return 7;
        case Bot::DINING_ROOM: return 8;
        case Bot::LIVING_ROOM: return 9;
        case Bot::COURTYARD: return 1;
        case Bot::GARAGE: return 2;
        case Bot::GAMES_ROOM: return 3;
    }

    return 0;
}

Bot::Room getPosRoom(int pos)
{
    switch (pos) {
        case 1: return Bot::COURTYARD;
        case 2: return Bot::GARAGE;
        case 3: return Bot::GAMES_ROOM;
        case 4: return Bot::BEDROOM;
        case 5: return Bot::BATHROOM;
        case 6: return Bot::STUDY;
        case 7: return Bot::KITCHEN;
        case 8: return Bot::DINING_ROOM;
        case 9: return Bot::LIVING_ROOM;
    }

    return Bot::COURTYARD;
}

int run()
{
    // make a deck of all the cards
    std::vector<Bot::Card> cards;

    for (int i = 0; i <= int(Bot::MAX_PLAYER); i++)
        cards.push_back(Bot::Player(i));
    for (int i = 0; i <= int(Bot::MAX_WEAPON); i++)
        cards.push_back(Bot::Weapon(i));
    for (int i = 0; i <= int(Bot::MAX_ROOM); i++)
        cards.push_back(Bot::Room(i));

    const int CARD_COUNT = 21;
    const unsigned int PLAYER_COUNT = 4 + (rand() % 3);
    const unsigned int TABLE_CARDS = (CARD_COUNT - 3) % PLAYER_COUNT;

    const unsigned int CARDS_PER_PLAYER = (CARD_COUNT - 3 - TABLE_CARDS) / PLAYER_COUNT;

    // choose the order for the players
    std::vector<Bot::Player> order;

    for (int i = 0; i <= int(Bot::MAX_PLAYER); i++)
        order.push_back(Bot::Player(i));

    while (order.size() > PLAYER_COUNT)
        order.erase(order.begin() + (rand() % order.size()));

    std::vector<Bot::Player> norder;
    for (int i = order.size() - 1; i >= 0; i--) {
        auto p = order[rand() % order.size()];
        norder.push_back(p);
        erase(order, p);
    }

    order = norder;

    // choose the envelope cards
    Bot::Player envelopePlayer = Bot::Player(rand() % (int(Bot::MAX_PLAYER) + 1));
    Bot::Weapon envelopeWeapon = Bot::Weapon(rand() % (int(Bot::MAX_WEAPON) + 1));
    Bot::Room envelopeRoom = Bot::Room(rand() % (int(Bot::MAX_ROOM) + 1));

    erase(cards, Bot::Card(envelopePlayer));
    erase(cards, Bot::Card(envelopeWeapon));
    erase(cards, Bot::Card(envelopeRoom));

    REQUIRE_FALSE(contains(cards, Bot::Card(envelopePlayer)));
    REQUIRE_FALSE(contains(cards, Bot::Card(envelopeWeapon)));
    REQUIRE_FALSE(contains(cards, Bot::Card(envelopeRoom)));

    // select the deck of cards for each player
    std::map<Bot::Player, std::vector<Bot::Card>> decks;

    for (auto p : order) {
        while (decks[p].size() < CARDS_PER_PLAYER) {
            Bot::Card c = cards[rand() % cards.size()];
            erase(cards, c);
            decks[p].push_back(c);
        }
    }

    std::map<Bot::Player, Bot*> players;

    std::map<Bot::Player, int> board;

    auto genBoard = [&]() {
        std::vector<std::pair<Bot::Player, Position>> b;
        for (auto p : order)
            b.push_back({ p, Position(board[p]) });
        return b;
    };

    // create the players

    for (auto p : order)
        board[p] = 0; // all players start in the middle room

    for (auto p : order) {
        players[p] = new Bot(p, order);
        players[p]->setCards(decks[p]); // player's cards
        players[p]->setCards(cards, true); // table cards
        players[p]->updateBoard(genBoard());
    }


    // play the game
    bool done = false;
    unsigned int curIndex = 0;
    unsigned int turns = 0;

    std::map<Bot::Player, int> tc;

    while (!done) {
        // select current player and dice roll
        Bot::Player cur = order[curIndex];
        int dice = 2 + (rand() % 11);
        int pos = players[cur]->getMove(dice);
        tc[cur]++;

        // check that the player gave a valid move
        Position start(board[cur]);
        Position end(pos);

        REQUIRE(int(start.path(end, 1)) <= dice);

        // move player (updates all the bots)
        for (auto o : order)
            players[o]->movePlayer(cur, pos);
        board[cur] = pos;

        if (pos < Board::ROOM_COUNT) { // going in to a room
            Bot::Suggestion sug = players[cur]->getSuggestion(); // get the suggestion
            if (pos == 0) { // it's making an accusation
                REQUIRE(sug.player == envelopePlayer);
                REQUIRE(sug.weapon == envelopeWeapon);
                REQUIRE(sug.room == envelopeRoom);

                done = true;
                break;
            }

            REQUIRE(sug.room == getPosRoom(pos));

            // move the player in the suggestion to the suggestion room
            if (contains(order, sug.player))
                board[sug.player] = pos;

            // notify all the players that the currently active bot made a suggestion (note that the
            // current bot is skipped)
            for (auto o : order)
                if (cur != o)
                    players[o]->madeSuggestion(cur, sug);

            // determine what player can show a card(s)
            std::vector<Bot::Card> show;
            Bot::Player other;
            for (unsigned int i = curIndex + 1; i != curIndex; i++) {
                if (i >= PLAYER_COUNT) {
                    i = 0;
                    if (i == curIndex)
                        break;
                }

                if (contains(decks[order[i]], Bot::Card(sug.player)))
                    show.push_back(Bot::Card(sug.player));
                if (contains(decks[order[i]], Bot::Card(sug.weapon)))
                    show.push_back(Bot::Card(sug.weapon));
                if (contains(decks[order[i]], Bot::Card(sug.room)))
                    show.push_back(Bot::Card(sug.room));

                if (!show.empty()) {
                    other = order[i];
                    break;
                }
            }

            if (show.empty()) { // nobody could show a card
                players[cur]->noShowCard(); // notify the currently active bot
                for (auto o : order) // notify all the others (note the current bot is skipped)
                    if (o != cur)
                        players[o]->noOtherShownCard();
            } else { // somebody could show a card
                Bot::Card c(cur);
                if (show.size() > 1) { // multiple cards to choose from, ask bot to show one
                    c = players[other]->getCard(cur, show);
                    REQUIRE(contains(show, c));
                } else
                    c = show[0];

                players[cur]->showCard(other, c); // show the card to the active bot

                // notify all the other bots (note that the current bot is skipped)
                for (auto o : order)
                    if (o != cur)
                        players[o]->otherShownCard(other);
            }
        }

        curIndex = (curIndex + 1) % PLAYER_COUNT;
        turns++;

        // notify all the bots that a new turn is starting
        for (auto o : order)
            players[o]->newTurn();
    }

    for (auto p : order)
        delete players[p];

    return tc[order[curIndex]];
}

TEST_CASE("game playthrough", "[.][game]") {
    std::map<int, int> turns;
    const int runs = 1000;
    for (int i = 0; i < runs; i++)
        turns[run()]++;

    int total = 0;
    for (auto t : turns)
        total += t.first * t.second;

    std::cout << "average turns needed to finish game: " << total / double(runs) << std::endl;
    std::cout << "minimum turns needed to finish game: " << turns.begin()->first << std::endl;
    std::cout << "maximum turns needed to finish game: " << turns.rbegin()->first << std::endl;

    total = 0;
    std::cout << "=========================\n";
    for (auto t : turns) {
        total += t.second;
        std::cout << t.first << ": " << t.second << "\t(" << t.second * 100 / double(runs) <<
            "%)\t(" << total * 100 / double(runs) << "%)\n";
    }
    std::cout << "=========================\n";
}

// vim: set expandtab textwidth=100:
