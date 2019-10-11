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

/**
 * \brief A reference bot
 *
 * This bot doesn't implement any fancy deductions or predictions, nor does it look at the history
 * of the game. This bot is a more realistic representation of how a human player will play. It
 * still follows a set strategy, it just doesn't extract as much information from the game as the
 * actual AI.
 */
class DumbBot
{
    public:
        DumbBot(Bot::Player player) :
            player(player),
            pos(0)
        {
            for (int i = 0; i <= int(Bot::MAX_PLAYER); i++)
                notes[Bot::Player(i)] = false;
            for (int i = 0; i <= int(Bot::MAX_WEAPON); i++)
                notes[Bot::Weapon(i)] = false;
            for (int i = 0; i <= int(Bot::MAX_ROOM); i++)
                notes[Bot::Room(i)] = false;
        }

        Bot::Suggestion getSuggestion()
        {
            Bot::Suggestion sug(Bot::Player(0), Bot::Weapon(0), Bot::Room(0));
            if (pos == 0) {
                sug.player = envelopePlayer;
                sug.weapon = envelopeWeapon;
                sug.room = envelopeRoom;
            } else {
                auto safePlayers = getSafePlayers();
                auto safeWeapons = getSafeWeapons();
                auto wantedPlayers = getWantedPlayers();
                auto wantedWeapons = getWantedWeapons();

                sug.room = getPosRoom(pos);

                if (!haveRoom) {
                    if (safePlayers.empty())
                        sug.player = wantedPlayers[rand() % wantedPlayers.size()];
                    else
                        sug.player = safePlayers[rand() % safePlayers.size()];

                    if (safeWeapons.empty())
                        sug.weapon = wantedWeapons[rand() %  wantedWeapons.size()];
                    else
                        sug.weapon = safeWeapons[rand() % safeWeapons.size()];
                } else if (!havePlayer) {
                    sug.player = wantedPlayers[rand() % wantedPlayers.size()];

                    if (safeWeapons.empty())
                        sug.weapon = wantedWeapons[rand() %  wantedWeapons.size()];
                    else
                        sug.weapon = safeWeapons[rand() % safeWeapons.size()];
                } else {
                    sug.player = safePlayers[rand() % safePlayers.size()];
                    sug.weapon = wantedWeapons[rand() %  wantedWeapons.size()];
                }
            }

            curSug = sug;
            return sug;
        }

        int getMove(int moves)
        {
            auto findRoom = [&](std::vector<Bot::Room> wanted) {
                for (auto w : wanted)
                    if (Position(this->pos).path(getRoomPos(w)) <= moves)
                        return getRoomPos(w);

                Bot::Room minRoom = wanted[0];
                int dist = Position(this->pos).path(getRoomPos(wanted[0]));

                for (auto w : wanted) {
                    int d = Position(this->pos).path(getRoomPos(w));
                    if (d < dist) {
                        dist = d;
                        minRoom = w;
                    }
                }

                return Position(pos).path(getRoomPos(minRoom)).partial(moves);
            };

            if (!haveRoom) {
                return findRoom(getWantedRooms());
            } else if (!havePlayer || !haveWeapon) {
                return findRoom(getSafeRooms());
            } else {
                return Position(pos).path(0).partial(moves);
            }
        }

        void setCards(std::vector<Bot::Card> cards)
        {
            for (auto c : cards) {
                notes[c] = true;
                set.push_back(c);
            }
            findEnvelope();
        }

        void showCard(Bot::Card card)
        {
            notes[card] = true;
            findEnvelope();
        }

        void move(int p)
        {
            this->pos = p;
        }

        void noShowCard()
        {
            if (!haveRoom && !contains(set, Bot::Card(curSug.room))) {
                haveRoom = true;
                envelopeRoom = curSug.room;
            }

            if (!haveWeapon && !contains(set, Bot::Card(curSug.weapon))) {
                haveWeapon = true;
                envelopeWeapon = curSug.weapon;
            }

            if (!havePlayer && !contains(set, Bot::Card(curSug.player))) {
                havePlayer = true;
                envelopePlayer = curSug.player;
            }
        }

        Bot::Card getCard(std::vector<Bot::Card> cards)
        {
            return cards[rand() % cards.size()];
        }

    private:
        template <typename T>
        std::pair<bool, T> finder(T max)
        {
            bool found = false;
            T envc;
            for (int i = 0; i <= int(max); i++) {
                T c = T(i);
                if (!notes[c]) {
                    if (!found) {
                        found = true;
                        envc = c;
                    } else {
                        found = false;
                        break;
                    }
                }
            }

            return std::make_pair(found, envc);
        }

        void findEnvelope()
        {
            if (!haveRoom) {
                auto p = finder(Bot::MAX_ROOM);
                haveRoom = p.first;
                envelopeRoom = p.second;
            }

            if (!haveWeapon) {
                auto p = finder(Bot::MAX_WEAPON);
                haveWeapon = p.first;
                envelopeWeapon = p.second;
            }

            if (!havePlayer) {
                auto p = finder(Bot::MAX_PLAYER);
                havePlayer = p.first;
                envelopePlayer = p.second;
            }
        }

        std::vector<Bot::Room> getWantedRooms()
        {
            std::vector<Bot::Room> wanted;
            for (int i = 0; i <= int(Bot::MAX_ROOM); i++)
                if (!notes[Bot::Room(i)])
                    wanted.push_back(Bot::Room(i));

            return wanted;
        }

        std::vector<Bot::Player> getWantedPlayers()
        {
            std::vector<Bot::Player> wanted;
            for (int i = 0; i <= int(Bot::MAX_PLAYER); i++)
                if (!notes[Bot::Player(i)])
                    wanted.push_back(Bot::Player(i));

            return wanted;
        }

        std::vector<Bot::Weapon> getWantedWeapons()
        {
            std::vector<Bot::Weapon> wanted;
            for (int i = 0; i <= int(Bot::MAX_WEAPON); i++)
                if (!notes[Bot::Weapon(i)])
                    wanted.push_back(Bot::Weapon(i));

            return wanted;
        }

        std::vector<Bot::Room> getSafeRooms()
        {
            std::vector<Bot::Room> safe;
            for (auto c : set)
                if (c.type == Bot::Card::ROOM)
                    safe.push_back(Bot::Room(c.card));

            if (safe.empty() && haveRoom)
                safe.push_back(envelopeRoom);

            return safe;
        }

        std::vector<Bot::Weapon> getSafeWeapons()
        {
            std::vector<Bot::Weapon> safe;
            for (auto c : set)
                if (c.type == Bot::Card::WEAPON)
                    safe.push_back(Bot::Weapon(c.card));

            if (safe.empty() && haveWeapon)
                safe.push_back(envelopeWeapon);

            return safe;
        }

        std::vector<Bot::Player> getSafePlayers()
        {
            std::vector<Bot::Player> safe;
            for (auto c : set)
                if (c.type == Bot::Card::PLAYER)
                    safe.push_back(Bot::Player(c.card));

            if (safe.empty() && havePlayer)
                safe.push_back(envelopePlayer);

            return safe;
        }

        std::map<Bot::Card, bool> notes;
        std::vector<Bot::Card> set;
        Bot::Player player;
        int pos;
        Bot::Player envelopePlayer;
        bool havePlayer = false;
        Bot::Weapon envelopeWeapon;
        bool haveWeapon = false;
        Bot::Room envelopeRoom;
        bool haveRoom = false;
        Bot::Suggestion curSug = Bot::Suggestion(Bot::Player(0), Bot::Weapon(0), Bot::Room(0));
};

class Player {
    public:
        Player(Bot::Player p, std::vector<Bot::Player> order, bool dumb) :
            player(p)
        {
            this->dumb = dumb;
            if (dumb)
                dbot = new DumbBot(p);
            else
                bot = new Bot(p, order);
        }

        ~Player()
        {
            delete bot;
            delete dbot;
        }

        void setCards(std::vector<Bot::Card> cards, bool table = false)
        {
            if (dumb)
                dbot->setCards(cards);
            else
                bot->setCards(cards, table);
        }

        void updateBoard(std::vector<std::pair<Bot::Player, Position>> players)
        {
            if (!dumb)
                bot->updateBoard(players);
        }

        int getMove(int dice)
        {
            if (dumb)
                return dbot->getMove(dice);
            else
                return bot->getMove(dice);
        }

        void movePlayer(Bot::Player p, int pos)
        {
            if (dumb) {
                if (player == p)
                    dbot->move(pos);
            } else
                bot->movePlayer(p, pos);
        }

        Bot::Suggestion getSuggestion()
        {
            if (dumb)
                return dbot->getSuggestion();
            else
                return bot->getSuggestion();
        }

        void madeSuggestion(Bot::Player player, Bot::Suggestion sug)
        {
            if (dumb) {
                if (sug.player == this->player)
                    dbot->move(getRoomPos(sug.room));
            } else
                bot->madeSuggestion(player, sug);
        }

        void noShowCard()
        {
            if (dumb)
                dbot->noShowCard();
            else
                bot->noShowCard();
        }

        void noOtherShownCard()
        {
            if (!dumb)
                bot->noOtherShownCard();
        }

        Bot::Card getCard(Bot::Player p, std::vector<Bot::Card> cards)
        {
            if (dumb)
                return dbot->getCard(cards);
            else
                return bot->getCard(p, cards);
        }

        void showCard(Bot::Player player, Bot::Card card)
        {
            if (dumb)
                dbot->showCard(card);
            else
                bot->showCard(player, card);
        }

        void otherShownCard(Bot::Player other)
        {
            if (!dumb)
                bot->otherShownCard(other);
        }

        void newTurn()
        {
            if (!dumb)
                bot->newTurn();
        }


    private:
        bool dumb;
        DumbBot* dbot = nullptr;
        Bot* bot = nullptr;
        Bot::Player player;
};

std::pair<bool, int> run()
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

    std::map<Bot::Player, Player*> players;

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

    Bot::Player smart = order[rand() % order.size()];
    bool wrong;

    for (auto p : order) {
        players[p] = new Player(p, order, p != smart);
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

                wrong = cur != smart;

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

    return std::make_pair(!wrong, tc[order[curIndex]]);
}

TEST_CASE("game playthrough", "[.][game]") {
    std::map<int, int> turns;
    int runs = 1000;
    int lost = 0;
    for (int i = 0; i < runs; i++) {
        auto t = run();
        if (t.first)
            turns[t.second]++;
        else
            lost++;
    }

    runs -= lost;

    int total = 0;
    int perc = 0;
    int third;
    bool tset = false;
    for (auto t : turns) {
        total += t.first * t.second;
        perc += t.second;
        if (((perc * 100 / runs) >= 75) && !tset) {
            third = t.first;
            tset = true;
        }
    }

    std::cout << std::setprecision(3);

    double won = double(runs) * 100 / (runs + lost);

    std::cout << "games won: " << runs << " (" << won << "%) (x" << won / 20 << " better than reference)\n";
    std::cout << "average turns needed to finish game: " << total / double(runs) << std::endl;
    std::cout << "minimum turns needed to finish game: " << turns.begin()->first << std::endl;
    std::cout << "maximum turns needed to finish game: " << turns.rbegin()->first << std::endl;
    std::cout << "third percentile: " << third << " moves needed" << std::endl;

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
