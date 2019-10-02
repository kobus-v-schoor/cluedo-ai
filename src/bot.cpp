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
    haveSuggestion = true;
    weMadeSuggestion = false;
    Deck deck = getWantedDeck();
    runPredictors(deck);

    deck.sort();

    std::vector<bool> occupied(Board::BOARD_SIZE, false);
    for (auto p : board)
        occupied[p.second] = true;

    if (!envelope.haveRoom) {
    } else if (!envelope.havePlayer) {
    } else if (!envelope.haveWeapon) {
    } else {
        Position dest(0);
        Position cur(board[player]);
        Position::Path path = cur.path(dest, occupied, 1);
        return path.partial(allowedMoves);
    }

    return 0;
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

Bot::Card Bot::getCard(Player player, std::vector<Card> cards){ return Card(Player(0)); }

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

Bot::Player Bot::choosePlayerOffensive(std::vector<Player> choices, Bot::Room room)
{
    std::vector<Player> seen;

    auto playerInChoices = [&](Player player) { return std::find(choices.begin(), choices.end(),
            player) != choices.end(); };
    auto playerInOrder = [&](Player player) { return std::find(order.begin(), order.end(),
            player) != order.end(); };

    bool found = false;
    for (auto c : choices) {
        if (playerInOrder(c)) {
            found = true;
            break;
        }
    }

    // if there is not a player that is currently playing and one of our choices, we cannot make an
    // offensive move and hence we can just return a random choice
    if (!found)
        return choices[rand() % choices.size()];

    for (auto p : order)
        if (notes[p][room].seen && playerInChoices(p))
            seen.push_back(p);

    // calculate the amount of rooms the player has possibly seen
    std::map<Player, int> seenCount;

    for (auto p : seen.empty() ? order : seen) {
        if (!seen.empty() || playerInChoices(p)) {
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
