/**
 * \file bot.cpp
 * \author Kobus van Schoor
 */

#include <algorithm>

#include "../include/bot.h"
#include "../include/board.h"

// deductors
#include "../include/deductor.h"
#include "../include/deductors/local-exclude.h"
#include "../include/deductors/no-show.h"
#include "../include/deductors/seen.h"

// predictors
#include "../include/deck.h"
#include "../include/predictor.h"
#include "../include/predictors/seen.h"

using namespace AI;

// checks if a vector contains an object
template <typename T>
bool contains(std::vector<T>& vec, T obj)
{
    return std::find(vec.begin(), vec.end(), obj) != vec.end();
}

Bot::Card::Card(Bot::Player p):
    card(int(p)),
    type(Card::Type::PLAYER)
{}

Bot::Card::Card(Bot::Weapon w):
    card(int(w)),
    type(Card::Type::WEAPON)
{}

Bot::Card::Card(Bot::Room r):
    card(int(r)),
    type(Card::Type::ROOM)
{}

bool Bot::Card::operator<(const Card& other) const
{
    return type == other.type ? card < other.card : type < other.type;
}

bool Bot::Card::operator==(const Card& other) const
{
    return (type == other.type) && (card == other.card);
}

Bot::Suggestion::Suggestion(Player p, Weapon w, Room r):
    player(p),
    weapon(w),
    room(r)
{}

bool Bot::Suggestion::operator==(const Suggestion& other)
{
    return (player == other.player) && (weapon == other.weapon) && (room == other.room);
}

bool Bot::Notes::concluded()
{
    return has || lacks;
}

bool Bot::Notes::knows()
{
    return has || seen;
}

void Bot::SuggestionLog::addSuggestion(Player from, Suggestion sug)
{
    if (waiting())
        throw std::runtime_error("already waiting for show");
    staging.suggestion = sug;
    staging.from = from;
    waitingForShow = true;
}

void Bot::SuggestionLog::addShow(Player player)
{
    if (!waiting()) {
        LOG_ERR("SuggestionLog modified in wrong order - AI was probably notified of a suggestion"
                << " in the wrong order");
        throw std::runtime_error("suggestion hasn't been set");
    }

    staging.show = player;
    staging.showed = true;
    _log.push_back(staging);
    waitingForShow = false;
}

void Bot::SuggestionLog::addNoShow()
{
    if (!waiting()) {
        LOG_ERR("SuggestionLog modified in wrong order - AI was probably notified of a suggestion"
                << " in the wrong order");
        throw std::runtime_error("suggestion hasn't been set");
    }
    staging.showed = false;
    _log.push_back(staging);
    waitingForShow = false;
}

bool Bot::SuggestionLog::waiting()
{
    return waitingForShow;
}

void Bot::SuggestionLog::clear()
{
    if (waiting())
        LOG_ERR("Suggestion log cleared while waiting for player - AI probably wasn't notified what"
                << " happened after a suggestion was made");
    waitingForShow = false;
}

std::vector<Bot::SuggestionLogItem> Bot::SuggestionLog::log()
{
    return _log;
}

bool Bot::Envelope::operator!=(const Envelope& other)
{
    return (havePlayer != other.havePlayer) || (haveWeapon != other.haveWeapon) ||
        (haveRoom != other.haveRoom);
}

Bot::Bot(const Player player, std::vector<Player> order) :
    player(player),
    order(order),
    curSuggestion(Player(0), Weapon(0), Room(0))
{
    // creates a notes entry for every player
    for (auto o : order)
        notes[o];

    // sets all cards as lacking for this player
    for (int i = 0; i <= int(MAX_PLAYER); i++)
        notes[player][Player(i)].lacks = true;
    for (int i = 0; i <= int(MAX_WEAPON); i++)
        notes[player][Weapon(i)].lacks = true;
    for (int i = 0; i <= int(MAX_ROOM); i++)
        notes[player][Room(i)].lacks = true;

    deductors.push_back(new LocalExcludeDeductor());
    deductors.push_back(new NoShowDeductor(order));
    deductors.push_back(new SeenDeductor());

    predictors.push_back(new SeenPredictor());
}

Bot::~Bot()
{
    for (auto d : deductors)
        delete d;
    for (auto p : predictors)
        delete p;
}

void Bot::setCards(const std::vector<Card> cards)
{
    for (auto c : cards) {
        notes[player][c].has = true;
        notes[player][c].lacks = false;
    }

    notesHook();
}

void Bot::updateBoard(const std::vector<std::pair<Player, Position>> players)
{
    board.clear();
    for (auto p : players)
        board[p.first] = p.second;
}

void Bot::movePlayer(const Player player, Position position)
{
    board[player] = position;
}

void Bot::madeSuggestion(Player player, Suggestion suggestion, bool accuse)
{
    log.addSuggestion(player, suggestion);
}

void Bot::otherShownCard(Player showed)
{
    log.addShow(showed);
    notesHook(true);
}

void Bot::noOtherShownCard()
{
    log.addNoShow();
    notesHook(true);
}

int Bot::getMove(int allowedMoves)
{
    Deck deck = getWantedDeck();
    runPredictors(deck);

    deck.sort();

    auto safePlayers = getSafePlayers();
    auto safeWeapons = getSafeWeapons();
    auto safeRooms = getSafeRooms();

    auto forcedRoom = [&](Room room) {
        if (deck.players.empty() && deck.weapons.empty()) { // we know both, annoy a player
            curSuggestion.player = choosePlayerOffensive(order, room);
            curSuggestion.weapon = Weapon(rand() % (int(MAX_WEAPON) + 1));
        } else if (deck.players.empty()) { // we know the player, choose a good weapon
            curSuggestion.player = choosePlayerOffensive(safePlayers, room);
            curSuggestion.weapon = deck.weapons[0];
        } else if (deck.weapons.empty()) { // we know the weapon, choose a good player
            curSuggestion.player = deck.players[0];
            curSuggestion.weapon = safeWeapons[rand() % safeWeapons.size()];
        } else { // we don't anything, choose good player and weapon
            curSuggestion.player = deck.players[0];
            curSuggestion.weapon = deck.weapons[0];
        }
    };

    if (!envelope.haveRoom) {
        int pos = findNextMove(allowedMoves, deck.rooms, true);

        if (pos < Board::ROOM_COUNT) { // we're entering a room
            Room room = getPosRoom(pos);
            curSuggestion.room = room;
            haveSuggestion = true;

            if (contains(deck.rooms, room)) { // we're entering a wanted room
                if (safePlayers.empty())
                    curSuggestion.player = deck.players[0];
                else
                    curSuggestion.player = choosePlayerOffensive(safePlayers, room);

                if (safeWeapons.empty())
                    curSuggestion.weapon = deck.weapons[0];
                else
                    curSuggestion.weapon = safeWeapons[rand() % safeWeapons.size()];
            } else // we're entering an unwanted room
                forcedRoom(room);
        } else // we're going/staying to a tile
            haveSuggestion = false;

        return pos;
    } else if (!envelope.havePlayer || !envelope.haveWeapon) {
        int pos = findNextMove(allowedMoves, safeRooms, true);
        int dest = board[this->player];

        // if we are already in a safe room, only change if the new destination is also a safe room
        if ((board[this->player] < Board::ROOM_COUNT) && contains(safeRooms,
                        getPosRoom(board[this->player]))) {
            if ((pos < Board::ROOM_COUNT) && contains(safeRooms, getPosRoom(pos)))
                dest = pos;
        } else { // we're not currently in a safe room
            // we're in the envelope room, so technically safe but we should move away from here if
            // we can get directly to another safe room to subvert suspicion
            if ((board[this->player] < Board::ROOM_COUNT) &&
                    (envelope.room == getPosRoom(board[this->player]))) {
                if ((pos < Board::ROOM_COUNT) && contains(safeRooms, getPosRoom(pos)))
                    dest = pos;
            } else // where not safe, go to the destination regardless
                dest = pos;
        }


        if (dest >= Board::ROOM_COUNT) { // new destination isn't a room
            haveSuggestion = false;
        } else {
            Room room = getPosRoom(dest);
            curSuggestion.room = room;
            haveSuggestion = true;

            // new room is a safe room, we can isolate our wanted card
            if (contains(safeRooms, room)) {
                if (!envelope.havePlayer) { // we're looking for a player
                    curSuggestion.player = deck.players[0];

                    if (safeWeapons.empty())
                        curSuggestion.weapon = deck.weapons[0];
                    else
                        curSuggestion.weapon = safeWeapons[rand() % safeWeapons.size()];
                } else { // we're looking for a weapon
                    curSuggestion.player = choosePlayerOffensive(safePlayers, room);
                    curSuggestion.weapon = deck.weapons[0];
                }
            } else // we're entering an unwanted room
                forcedRoom(room);
        }

        return dest;
    } else { // we want to make an accusation
        std::vector<bool> occupied(Board::BOARD_SIZE, false);
        for (auto p : board)
            occupied[p.second] = true;

        Position::Path path(board[this->player]);
        bool blocked = false;

        try {
            path = Position(board[this->player]).path(0, occupied, 1);
        } catch (std::runtime_error&) { // we're blocked
            blocked = true;
        }

        int dest;

        if (blocked) { // we're blocked by another player
            blocked = false;

            // check if we can get around the blockage
            try {
                path = Position(board[this->player]).path(0, occupied, Board::ROOM_COUNT);
            } catch (std::runtime_error&) {
                blocked = true;
            }

            if (blocked) // we're completely stuck, we need to stay where we are
                dest = board[this->player];
            else // we can get around the blockage, get as far as we can
                dest = path.partial(allowedMoves);
        } else // we're not blocked, try and get to the middle
            dest = path.partial(allowedMoves);

        if (dest == 0) { // we're going to middle, get ready to accuse
            curSuggestion.player = envelope.player;
            curSuggestion.weapon = envelope.weapon;
            curSuggestion.room = envelope.room;
            haveSuggestion = true;
        } else // not reaching the middle just yet
            haveSuggestion = false;

        return dest;
    }
}

Bot::Suggestion Bot::getSuggestion()
{
    if (!haveSuggestion)
        throw std::runtime_error("Suggestion hasn't been set because getMove wasn't called before "
                "getSuggestion() or we're currently outside of a room");
    LOG_LOGIC("Making suggestion " << curSuggestion);
    haveSuggestion = false;
    weMadeSuggestion = true;
    return curSuggestion;
}

void Bot::showCard(Player player, Card card)
{
    notes[this->player][card].seen = true;
    notes[player][card].has = true;
    if (weMadeSuggestion) {
        log.addSuggestion(this->player, curSuggestion);
        log.addShow(player);
        weMadeSuggestion = false;
    }
    notesHook();
}

void Bot::noShowCard()
{
    if (weMadeSuggestion) {
        log.addSuggestion(this->player, curSuggestion);
        log.addNoShow();
        notesHook(true);
        weMadeSuggestion = false;
    }
}

Bot::Card Bot::getCard(Player player, std::vector<Card> cards)
{
    // check if we can find a card that the player has already seen
    for (auto c : cards)
        if (notes[player][c].seen)
            return c;

    // find the card type the player knows the least about and show that type to them
    // this way we minimize the risk of giving them the info needed to find an envelope card
    std::vector<int> types(3, 0);

    for (auto c : notes[player])
        if (c.second.seen)
            types[int(c.first.type)]++;

    int min = 0;
    for (size_t i = 1; i < cards.size(); i++)
        if (types[int(cards[i].type)] < types[int(cards[min].type)])
            min = i;

    return cards[min];
}

void Bot::newTurn()
{
    log.clear();
}

std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>> Bot::getNotes()
{
    return notes;
}

void Bot::notesHook(bool nolacking)
{
    if (!nolacking)
        notesMarkLacking();
    runDeductors();
    findEnvelope();
}

void Bot::runDeductors()
{
    bool made;
    int count = 0;
    do {
        made = false;
        for (auto d : deductors)
            if (d->run(log, notes))
                made = true;

        if (made)
            notesMarkLacking();

        count++;
    } while (made && (count < MAX_DEDUCTOR_RUN_COUNT));
}

void Bot::runPredictors(Deck& deck)
{
    for (auto p : predictors)
        p->run(deck, notes, log);
}

void Bot::notesMarkLacking()
{
    for (auto player : order) {
        for (auto card : notes[player]) {
            if (card.second.has) {
                for (auto otherp : order) {
                    if (otherp == player)
                        continue;
                    notes[otherp][card.first].lacks = true;
                }
            }
        }
    }
}

bool Bot::findEnvelope()
{
    Envelope env = envelope;

    // for all the cards, first check if there is a card that everyone lacks. if it cannot find such
    // a card, check if we know that everyone has all the cards except for one

    if (!envelope.havePlayer) {
        for (int i = 0; (i <= int(MAX_PLAYER)); i++) {
            bool found = false;
            for (auto player : order) {
                if (!notes[player][Player(i)].lacks) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                envelope.player = Player(i);
                envelope.havePlayer = true;
                LOG_LOGIC("SOLVED: " << envelope.player << " is the envelope player (all-lacks)");
                break;
            }
        }

        if (!envelope.havePlayer) {
            bool noHas = false;
            Player noHasPlayer;

            for (int i = 0; i <= int(MAX_PLAYER); i++) {
                bool found = false;
                for (auto player : order) {
                    if (notes[player][Player(i)].has) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    // multiple cards hasn't been found
                    if (noHas) {
                        noHas = false;
                        break;
                    }
                    noHas = true;
                    noHasPlayer = Player(i);
                }
            }

            if (noHas) {
                envelope.player = noHasPlayer;
                envelope.havePlayer = true;
                LOG_LOGIC("SOLVED: " << envelope.player << " is the envelope player (no-has)");
            }
        }
    }

    if (!envelope.haveWeapon) {
        for (int i = 0; (i <= int(MAX_WEAPON)); i++) {
            bool found = false;
            for (auto player : order) {
                if (!notes[player][Weapon(i)].lacks) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                envelope.weapon = Weapon(i);
                envelope.haveWeapon = true;
                LOG_LOGIC("SOLVED: " << envelope.weapon << " is the envelope weapon (all-lacks)");
                break;
            }
        }

        if (!envelope.haveWeapon) {
            bool noHas = false;
            Weapon noHasWeapon;

            for (int i = 0; i <= int(MAX_WEAPON); i++) {
                bool found = false;
                for (auto player : order) {
                    if (notes[player][Weapon(i)].has) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    // multiple cards hasn't been found
                    if (noHas) {
                        noHas = false;
                        break;
                    }
                    noHas = true;
                    noHasWeapon = Weapon(i);
                }
            }

            if (noHas) {
                envelope.weapon = noHasWeapon;
                envelope.haveWeapon = true;
                LOG_LOGIC("SOLVED: " << envelope.weapon << " is the envelope weapon (no-has)");
            }
        }
    }

    if (!envelope.haveRoom) {
        for (int i = 0; (i <= int(MAX_ROOM)); i++) {
            bool found = false;
            for (auto player : order) {
                if (!notes[player][Room(i)].lacks) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                envelope.room = Room(i);
                envelope.haveRoom = true;
                LOG_LOGIC("SOLVED: " << envelope.room << " is the envelope room (all-lacks)");
                break;
            }
        }

        if (!envelope.haveRoom) {
            bool noHas = false;
            Room noHasRoom;

            for (int i = 0; i <= int(MAX_ROOM); i++) {
                bool found = false;
                for (auto player : order) {
                    if (notes[player][Room(i)].has) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    // multiple cards hasn't been found
                    if (noHas) {
                        noHas = false;
                        break;
                    }
                    noHas = true;
                    noHasRoom = Room(i);
                }
            }

            if (noHas) {
                envelope.room = noHasRoom;
                envelope.haveRoom = true;
                LOG_LOGIC("SOLVED: " << envelope.room << " is the envelope room (no-has)");
            }
        }
    }

    return env != envelope;
}

Deck Bot::getWantedDeck()
{
    Deck deck;

    if (!envelope.havePlayer) {
        for (int i = 0; i <= int(MAX_PLAYER); i++) {
            bool found = false;
            for (auto p : order) {
                if (notes[p][Player(i)].has) {
                    found = true;
                    break;
                }
            }

            if (!found)
                deck.players.push_back(Player(i));
        }
    }

    if (!envelope.haveWeapon) {
        for (int i = 0; i <= int(MAX_WEAPON); i++) {
            bool found = false;
            for (auto p : order) {
                if (notes[p][Weapon(i)].has) {
                    found = true;
                    break;
                }
            }

            if (!found)
                deck.weapons.push_back(Weapon(i));
        }
    }

    if (!envelope.haveRoom) {
        for (int i = 0; i <= int(MAX_ROOM); i++) {
            bool found = false;
            for (auto p : order) {
                if (notes[p][Room(i)].has) {
                    found = true;
                    break;
                }
            }

            if (!found)
                deck.rooms.push_back(Room(i));
        }
    }

    return deck;
}

std::vector<Bot::Player> Bot::getSafePlayers()
{
    std::vector<Player> players;

    for (int i = 0; i <= int(MAX_PLAYER); i++)
        if (notes[player][Player(i)].has)
            players.push_back(Player(i));

    if (players.empty() && envelope.havePlayer)
        players.push_back(envelope.player);

    return players;
}

std::vector<Bot::Weapon> Bot::getSafeWeapons()
{
    std::vector<Weapon> weapons;

    for (int i = 0; i <= int(MAX_WEAPON); i++)
        if (notes[player][Weapon(i)].has)
            weapons.push_back(Weapon(i));

    if (weapons.empty() && envelope.haveWeapon)
        weapons.push_back(envelope.weapon);

    return weapons;
}

std::vector<Bot::Room> Bot::getSafeRooms()
{
    std::vector<Room> rooms;

    for (int i = 0; i <= int(MAX_ROOM); i++)
        if (notes[player][Room(i)].has)
            rooms.push_back(Room(i));

    if (rooms.empty() && envelope.haveRoom)
        rooms.push_back(envelope.room);

    return rooms;
}

int Bot::getRoomPos(Room room)
{
    switch (room) {
        case BEDROOM: return 4;
        case BATHROOM: return 5;
        case STUDY: return 6;
        case KITCHEN: return 7;
        case DINING_ROOM: return 8;
        case LIVING_ROOM: return 9;
        case COURTYARD: return 1;
        case GARAGE: return 2;
        case GAMES_ROOM: return 3;
    }

    return 0;
}

Bot::Room Bot::getPosRoom(int pos)
{
    switch (pos) {
        case 1: return COURTYARD;
        case 2: return GARAGE;
        case 3: return GAMES_ROOM;
        case 4: return BEDROOM;
        case 5: return BATHROOM;
        case 6: return STUDY;
        case 7: return KITCHEN;
        case 8: return DINING_ROOM;
        case 9: return LIVING_ROOM;
    }

    return COURTYARD;
}

int Bot::findNextMove(int allowedMoves, std::vector<Room> wanted, bool allowOccupied)
{
    // first check if we are already in a wanted room - if we are, remove it from the wanted list
    if ((board[this->player] < Board::ROOM_COUNT) && contains(wanted,
                getPosRoom(board[this->player])))
        wanted.erase(std::find(wanted.begin(), wanted.end(), getPosRoom(board[this->player])));

    std::vector<std::pair<Room, Position::Path>> dists;
    Position start(board[this->player]);

    std::vector<bool> occupied(Board::BOARD_SIZE, false);
    if (!allowOccupied)
        for (auto p : board)
            occupied[p.second] = true;

    // find the distances for all the wanted rooms
    for (auto w : wanted) {
        try {
            Position::Path path = start.path(getRoomPos(w), occupied, 1);
            dists.push_back({ w, path });
        } catch (std::runtime_error&) {}; // blocked
    }

    // check for all rooms that we can enter
    std::vector<int> unwantedRooms;
    for (int i = 1; i < Board::ROOM_COUNT; i++) {
        int dist;
        try {
            dist = start.path(i, occupied, 1);
        } catch (std::runtime_error&) {
            continue;
        }

        if (dist <= allowedMoves)
            unwantedRooms.push_back(i);
    }

    // will find the unwanted room that is closest to a wanted room (either by shortcut or by tiles)
    auto findBestUnwanted = [&]() {
        // <room pos, <wanted pos, wanted dist>>
        std::map<int, std::pair<int, int>> unwantedDistance;
        for (auto r : unwantedRooms) {
            std::map<int, int> wantedDistance;

            for (auto w : wanted) {
                int p = getRoomPos(w);
                wantedDistance[p] = Position(r).path(p, 1);
            }

            int min = wantedDistance.begin()->first;
            for (auto w : wantedDistance)
                if (w.second < wantedDistance[min])
                    min = w.first;

            unwantedDistance[r] = { min, wantedDistance[min] };
        }

        int min = unwantedDistance.begin()->first;
        for (auto u : unwantedDistance)
            if (u.second.second < unwantedDistance[min].second)
                min = u.first;

        return min;
    };

    // will follow the partial path up to the closest room
    auto findClosestRoom = [&]() {
        std::vector<Position::Path> paths;

        for (int i = 1; i < Board::ROOM_COUNT; i++) {
            try {
                Position::Path p = Position(board[this->player]).path(i, occupied, 1);
                paths.push_back(p);
            } catch (std::runtime_error&) { // blocked
                continue;
            }
        }

        Position::Path min = *paths.begin();

        for (auto p : paths)
            if (p < min)
                min = p;

        return min.partial(allowedMoves);
    };

    if (dists.empty()) { // everything was blocked in some way
        // first check if we can move at all
        bool fenced = true;
        for (auto n : start.getNeighbours()) {
            if (occupied[n])
                continue;
            fenced = false;
            break;
        }

        if (fenced) // we're completely stuck, so just stay here
            return board[this->player];

        if (unwantedRooms.empty()) { // we cannot reach anything
            // move towards the closest room
            return findClosestRoom();
        } else { // we can reach at least one unwanted room
            // move towards the unwanted room with the closest wanted room
            return findBestUnwanted();
        }
    } else { // some of the rooms were unblocked
        // first check if we can reach any of the rooms, if we can - go there
        for (auto d : dists)
            if (d.second <= allowedMoves)
                return getRoomPos(d.first);

        // if there is any unwanted rooms we can go in to, rather go there
        if (!unwantedRooms.empty())
            return findBestUnwanted();

        // no rooms were found that was in reach, go towards the closest room
        return findClosestRoom();
    }
}

Bot::Player Bot::choosePlayerOffensive(std::vector<Player> choices, Bot::Room room)
{
    std::vector<Player> seen;

    bool found = false;
    for (auto c : choices) {
        if (contains(order, c)) {
            found = true;
            break;
        }
    }

    // if there is not a player that is currently playing and one of our choices, we cannot make an
    // offensive move and hence we can just return a random choice
    if (!found)
        return choices[rand() % choices.size()];

    for (auto p : order)
        if (notes[p][room].seen && contains(choices, p))
            seen.push_back(p);

    // calculate the amount of rooms the player has possibly seen
    std::map<Player, int> seenCount;

    for (auto p : seen.empty() ? order : seen) {
        if (!seen.empty() || contains(choices, p)) {
            seenCount[p] = 0;
            for (int i = 0; i <= int(MAX_ROOM); i++)
                if (notes[p][Room(i)].seen)
                    seenCount[p]++;
        }
    }

    // if nobody has seen the room, return the player that has seen the least amount of rooms since
    // they are probably the least likely to find the room envelope card by being in the room.
    // if we have found somebody who has seen the room, find the player who has seen the most rooms
    // and move them to the room since they will probably be moved away from their target and
    // possibly hinder them in finding the envelope room
    if (seen.empty())
        return std::min_element(seenCount.begin(), seenCount.end(),
                [](std::pair<Player, int> a, std::pair<Player, int> b) {
                return a.second < b.second; })->first;
    else
        return std::max_element(seenCount.begin(), seenCount.end(),
                [](std::pair<Player, int> a, std::pair<Player, int> b) {
                return a.second < b.second; })->first;
}

std::ostream& operator<<(std::ostream& ostream, const AI::Bot::Player player)
{
    switch (player) {
        case Bot::SCARLET: ostream << "SCARLET"; break;
        case Bot::PLUM: ostream << "PLUM"; break;
        case Bot::PEACOCK: ostream << "PEACOCK"; break;
        case Bot::GREEN: ostream << "GREEN"; break;
        case Bot::MUSTARD: ostream << "MUSTARD"; break;
        case Bot::WHITE: ostream << "WHITE"; break;
    }

    return ostream;
}

std::ostream& operator<<(std::ostream& ostream, const AI::Bot::Weapon weapon)
{
    switch (weapon) {
        case Bot::CANDLESTICK: ostream << "CANDLESTICK"; break;
        case Bot::KNIFE: ostream << "KNIFE"; break;
        case Bot::LEAD_PIPE: ostream << "LEAD_PIPE"; break;
        case Bot::REVOLVER: ostream << "REVOLVER"; break;
        case Bot::ROPE: ostream << "ROPE"; break;
        case Bot::SPANNER: ostream << "SPANNER"; break;
    }

    return ostream;
}

std::ostream& operator<<(std::ostream& ostream, const AI::Bot::Room room)
{
    switch (room) {
        case Bot::BEDROOM: ostream << "BEDROOM"; break;
        case Bot::BATHROOM: ostream << "BATHROOM"; break;
        case Bot::STUDY: ostream << "STUDY"; break;
        case Bot::KITCHEN: ostream << "KITCHEN"; break;
        case Bot::DINING_ROOM: ostream << "DINING_ROOM"; break;
        case Bot::LIVING_ROOM: ostream << "LIVING_ROOM"; break;
        case Bot::COURTYARD: ostream << "COURTYARD"; break;
        case Bot::GARAGE: ostream << "GARAGE"; break;
        case Bot::GAMES_ROOM: ostream << "GAMES_ROOM"; break;
    }

    return ostream;
}

std::ostream& operator<<(std::ostream& ostream, const AI::Bot::Card card)
{
    switch (card.type) {
        case Bot::Card::PLAYER: ostream << Bot::Player(card.card); break;
        case Bot::Card::WEAPON: ostream << Bot::Weapon(card.card); break;
        case Bot::Card::ROOM: ostream << Bot::Room(card.card); break;
    }

    return ostream;
}

std::ostream& operator<<(std::ostream& ostream, const AI::Bot::Suggestion suggestion)
{
    ostream << "(P: " << suggestion.player << ", W: " << suggestion.weapon << ", R: " <<
        suggestion.room << ")";
    return ostream;
}

// vim: set expandtab textwidth=100:
