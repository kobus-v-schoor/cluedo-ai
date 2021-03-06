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
#include "../include/deductors/card-count-exclude.h"

// predictors
#include "../include/deck.h"
#include "../include/predictor.h"
#include "../include/predictors/seen.h"
#include "../include/predictors/multiple.h"
#include "../include/predictors/no-show.h"

using namespace AI;

// checks if a vector contains an object
template <typename T>
bool contains(std::vector<T>& vec, T obj)
{
    return std::find(vec.begin(), vec.end(), obj) != vec.end();
}

void toLower(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char a) { return
            std::tolower(a); });
}

Bot::Player Bot::strToPlayer(std::string s)
{
    toLower(s);

    if (s == "green")
        return GREEN;
    if (s == "mustard")
        return MUSTARD;
    if (s == "peacock")
        return PEACOCK;
    if (s == "plum")
        return PLUM;
    if (s == "scarlet")
        return SCARLET;
    if (s == "white")
        return WHITE;

    throw std::invalid_argument("\"" + s + "\" is not a valid player");
}

Bot::Weapon Bot::strToWeapon(std::string s)
{
    toLower(s);

    if (s == "candlestick")
        return CANDLESTICK;
    if ((s == "dagger") || (s == "knife"))
        return KNIFE;
    if (s == "lead pipe")
        return LEAD_PIPE;
    if ((s == "pistol") || (s == "revolver"))
        return REVOLVER;
    if (s == "rope")
        return ROPE;
    if ((s == "wrench") || (s == "spanner"))
        return SPANNER;

    throw std::invalid_argument("\"" + s + "\" is not a valid weapon");
}

Bot::Room Bot::strToRoom(std::string s)
{
    toLower(s);

    if (s == "courtyard")
        return COURTYARD;
    if (s == "garage")
        return GARAGE;
    if ((s == "game room") || (s == "games room") || (s == "billiard room"))
        return GAMES_ROOM;
    if (s == "bedroom")
        return BEDROOM;
    if (s == "bathroom")
        return BATHROOM;
    if (s == "study")
        return STUDY;
    if (s == "kitchen")
        return KITCHEN;
    if (s == "dining room")
        return DINING_ROOM;
    if (s == "living room")
        return LIVING_ROOM;

    throw std::invalid_argument("\"" + s + "\" is not a valid room");
}

std::string Bot::playerToStr(Player p)
{
    switch (p) {
        case GREEN: return "Green";
        case MUSTARD: return "Mustard";
        case PEACOCK: return "Peacock";
        case PLUM: return "Plum";
        case SCARLET: return "Scarlet";
        case WHITE: return "White";
    }

    return "";
}

std::string Bot::weaponToStr(Weapon w)
{
    switch (w) {
        case CANDLESTICK: return "Candlestick";
        case KNIFE: return "Dagger";
        case LEAD_PIPE: return "Lead Pipe";
        case REVOLVER: return "Pistol";
        case ROPE: return "Rope";
        case SPANNER: return "Wrench";
    }

    return "";
}

std::string Bot::roomToStr(Room r)
{
    switch (r) {
        case COURTYARD: return "Courtyard";
        case GARAGE: return "Garage";
        case GAMES_ROOM: return "Game Room";
        case BEDROOM: return "Bedroom";
        case BATHROOM: return "Bathroom";
        case STUDY: return "Study";
        case KITCHEN: return "Kitchen";
        case DINING_ROOM: return "Dining Room";
        case LIVING_ROOM: return "Living Room";
    }

    return "";
}

std::string Bot::enumToStr(Player p)
{
    return playerToStr(p);
}

std::string Bot::enumToStr(Weapon w)
{
    return weaponToStr(w);
}

std::string Bot::enumToStr(Room r)
{
    return roomToStr(r);
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

Bot::Card::Card(std::string s)
{
    try {
        Bot::Player p = strToPlayer(s);
        type = PLAYER;
        card = int(p);
    } catch (std::invalid_argument&) {
        try {
            Bot::Weapon w = strToWeapon(s);
            type = WEAPON;
            card = int(w);
        } catch (std::invalid_argument&) {
            Bot::Room r = strToRoom(s);
            type = ROOM;
            card = int(r);
        }
    }
}

bool Bot::Card::operator<(const Card& other) const
{
    return type == other.type ? card < other.card : type < other.type;
}

bool Bot::Card::operator==(const Card& other) const
{
    return (type == other.type) && (card == other.card);
}

std::string Bot::Card::str() const
{
    switch (type) {
        case PLAYER: return playerToStr(Bot::Player(card));
        case WEAPON: return weaponToStr(Bot::Weapon(card));
        case ROOM: return roomToStr(Bot::Room(card));
    }

    return "";
}

Bot::Card::operator std::string() const
{
    return this->str();
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

Bot::Suggestion::operator std::string()
{
    return "P: " + Bot::playerToStr(player) + " W: " + Bot::weaponToStr(weapon) + " R: " +
        Bot::roomToStr(room);
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
    std::lock_guard<std::mutex> l(lock);

    auto os = [&]() {
        std::string s = "[";
        for (auto o : order)
            s += playerToStr(o) + ",";
        s.pop_back();
        return s + "]";
    };

    LOG_INFO("bot created, order: " + os());

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

    deductors.push_back(new LocalExcludeDeductor(player));
    deductors.push_back(new NoShowDeductor(player, order));
    deductors.push_back(new SeenDeductor(player));
    deductors.push_back(new CardCountExcludeDeductor(player, order));

    predictors.push_back(new SeenPredictor(player));
    predictors.push_back(new MultiplePredictor(player));
    predictors.push_back(new NoShowPredictor(player));
}

Bot::~Bot()
{
    LOG_INFO("destroying bot");
    for (auto d : deductors)
        delete d;
    for (auto p : predictors)
        delete p;
}

void Bot::setCards(const std::vector<Card> cards, bool tableCards)
{
    std::lock_guard<std::mutex> l(lock);

    auto cs = [&](){
        std::string s = "[";
        for (auto c : cards)
            s += std::string(c) + ",";
        s.pop_back();
        return s + "]";
    };

    if (tableCards)
        LOG_INFO("setting table cards: " + cs());
    else
        LOG_INFO("setting private cards: " + cs());

    for (auto c : cards) {
        notes[player][c].has = true;
        notes[player][c].lacks = false;
        notes[player][c].table = tableCards;
    }

    if (tableCards)
        for (auto o : order)
            for (auto c : cards)
                notes[o][c].seen = true;

    notesHook();
}

void Bot::updateBoard(const std::vector<std::pair<Player, Position>> players)
{
    std::lock_guard<std::mutex> l(lock);

    auto bs = [&]() {
        std::string s = "[";
        for (auto p : players)
            s += playerToStr(p.first) + ":" + std::to_string(p.second) + ",";
        s.pop_back();
        return s + "]";
    };

    LOG_INFO("updating board: " + bs());

    board.clear();
    for (auto p : players)
        board[p.first] = p.second;
}

void Bot::movePlayer(const Player player, Position position)
{
    std::lock_guard<std::mutex> l(lock);

    LOG_INFO("moving player " + playerToStr(player) + " -> " + std::to_string(position));

    board[player] = position;
}

void Bot::madeSuggestion(Player player, Suggestion suggestion, bool accuse)
{
    std::lock_guard<std::mutex> l(lock);

    LOG_INFO("adding suggestion to log: " + std::string(suggestion));

    if (log.waiting())
        log.clear();
    log.addSuggestion(player, suggestion);
    if (contains(order, suggestion.player))
        board[suggestion.player] = getRoomPos(suggestion.room);
}

void Bot::otherShownCard(Player showed)
{
    std::lock_guard<std::mutex> l(lock);

    LOG_INFO("adding to log that " + playerToStr(showed) + " showed a card");

    if (log.waiting())
        log.addShow(showed);
    notesHook(true);
}

void Bot::noOtherShownCard()
{
    std::lock_guard<std::mutex> l(lock);

    LOG_INFO("adding to log that nobody was able to show a card");

    if (log.waiting())
        log.addNoShow();
    notesHook(true);
}

int Bot::getMove(int allowedMoves)
{
    std::lock_guard<std::mutex> l(lock);

    LOG_INFO("asked for move");

    Deck deck = getWantedDeck();
    runPredictors(deck);
    deck.sort();

    bool lookRoom = false;
    bool lookWP = false;

    if (!envelope.haveRoom)
        lookRoom = true;
    if (!envelope.havePlayer || !envelope.haveWeapon)
        lookWP = true;

    if (lookRoom && lookWP) {
        lookWP = true;
        lookRoom = false;
    }

    if (lookRoom) {
        LOG_LOGIC("moving to find room");
        return findNextMove(allowedMoves, deck.rooms, !OCCUPIED_BLOCKED);
    } else if (lookWP) {
        LOG_LOGIC("moving to find weapon or player");

        auto safeRooms = getSafeRooms();
        int pos = findNextMove(allowedMoves, safeRooms, !OCCUPIED_BLOCKED);
        int dest = board[this->player];

        // if we are already in a safe room, only change if the new destination is also a safe room
        if (board[this->player] == 0) {
            LOG_LOGIC("moving out of middle room regardless of planned destination");
            dest = pos;
        } else if ((board[this->player] < Board::ROOM_COUNT) && contains(safeRooms,
                    getPosRoom(board[this->player]))) {
            if ((pos < Board::ROOM_COUNT) && contains(safeRooms, getPosRoom(pos))) {
                LOG_LOGIC("can move to another safe room from current safe room, moving there");
                dest = pos;
            } else
                LOG_LOGIC("staying in current room because we're unable to reach a safe room");
        } else { // we're not currently in a safe room
            // we're in the envelope room, so technically safe but we should move away from here if
            // we can get directly to another safe room to subvert suspicion
            if ((board[this->player] < Board::ROOM_COUNT) &&
                    (envelope.room == getPosRoom(board[this->player]))) {
                if ((pos < Board::ROOM_COUNT) && contains(safeRooms, getPosRoom(pos))) {
                    dest = pos;
                    LOG_LOGIC("moving away from envelope room to safe room");
                } else
                    LOG_LOGIC("staying in safe (envelope) room");
            } else { // where not safe, go to the destination regardless
                dest = pos;
                LOG_LOGIC("not in safe room, moving to new destination");
            }
        }

        return dest;
    } else {
        std::vector<bool> occupied(Board::BOARD_SIZE, false);
        if (OCCUPIED_BLOCKED)
            for (auto p : board)
                occupied[p.second] = true;

        int pos = board[this->player];
        Position::Path path(pos);
        bool blocked = false;

        try {
            path = Position(pos).path(0, occupied, 1);
        } catch (std::runtime_error&) { // we're blocked
            blocked = true;
        }

        int dest;

        if (blocked) { // we're blocked by another player
            LOG_LOGIC("cannot get to middle room because we are blocked");
            blocked = false;

            // check if we can get around the blockage
            try {
                path = Position(pos).path(0, occupied, Board::ROOM_COUNT);
            } catch (std::runtime_error&) {
                blocked = true;
            }

            if (blocked) { // we're completely stuck, we need to stay where we are
                dest = pos;
                LOG_LOGIC("were completely blocked, staying in current position");
            } else {// we can get around the blockage, get as far as we can
                dest = path.partial(allowedMoves);
                LOG_LOGIC("trying to get around blockage");
            }
        } else { // we're not blocked, try and get to the middle
            LOG_LOGIC("we are not blocked, trying to reach middle room");
            dest = path.partial(allowedMoves);
        }

        return dest;
    }
}

Bot::Suggestion Bot::getSuggestion()
{
    std::lock_guard<std::mutex> l(lock);

    LOG_INFO("asked for suggestion");

    int pos = board[this->player];

    if (pos >= Board::ROOM_COUNT)
        throw std::runtime_error("cannot make suggestion if not in room (current pos " +
                std::to_string(pos) + ")");

    if ((pos == 0) && !(envelope.havePlayer && envelope.haveWeapon && envelope.haveRoom))
        LOG_ERR("Being forced to make accusation before ready");

    Deck deck = getWantedDeck();
    runPredictors(deck);
    deck.sort();

    auto safePlayers = getSafePlayers();
    auto safeWeapons = getSafeWeapons();
    auto safeRooms = getSafeRooms();

    Room room = pos == 0 ? MAX_ROOM : getPosRoom(pos);
    curSuggestion.room = room;

    auto chooseWP = [&]() {
        if (envelope.havePlayer && envelope.haveWeapon) { // we know both, annoy a player
            LOG_LOGIC("we know both the envelope weapon and player, playing offensively");
            curSuggestion.player = choosePlayerOffensive(order, room);
            curSuggestion.weapon = Weapon(rand() % (int(MAX_WEAPON) + 1));
        } else if (envelope.havePlayer) { // we know the player, choose a good weapon
            LOG_LOGIC("we know the envelope player, optimizing weapon choice");
            curSuggestion.player = choosePlayerOffensive(safePlayers, room);
            curSuggestion.weapon = deck.weapons[0];
        } else if (envelope.haveWeapon) { // we know the weapon, choose a good player
            LOG_LOGIC("we know the envelope weapon, optimizing player choice");
            curSuggestion.player = deck.players[0];
            curSuggestion.weapon = safeWeapons[rand() % safeWeapons.size()];
        } else { // we don't know anything, choose good player and weapon
            curSuggestion.player = deck.players[0];
            curSuggestion.weapon = deck.weapons[0];
        }
    };

    if (envelope.havePlayer && envelope.haveWeapon && envelope.haveRoom) {
        if (pos == 0) {
            LOG_LOGIC("in middle room, making accusation");
            curSuggestion.player = envelope.player;
            curSuggestion.weapon = envelope.weapon;
            curSuggestion.room = envelope.room;
        } else {
            LOG_LOGIC("not yet in middle room, not making accusation yet - now annoying other players");
            curSuggestion.room = getPosRoom(pos);
            curSuggestion.player = choosePlayerOffensive(order, curSuggestion.room);
            curSuggestion.weapon = Weapon(rand() % (MAX_WEAPON + 1));
        }
    } else if (contains(safeRooms, room)) {
        LOG_LOGIC("in safe room, trying to find weapon or player");
        chooseWP();
    } else {
        LOG_LOGIC("not in safe room");
        if (deck.contains(room)) {
            LOG_LOGIC("in a room which we're still uncertain about, trying to isolate room");

            if (safePlayers.empty())
                curSuggestion.player = deck.players[0];
            else
                curSuggestion.player = choosePlayerOffensive(safePlayers, room);

            if (safeWeapons.empty())
                curSuggestion.weapon = deck.weapons[0];
            else
                curSuggestion.weapon = safeWeapons[rand() % safeWeapons.size()];
        } else {
            LOG_LOGIC("in a room we already know about, trying to optimize weapon and player choice");
            chooseWP();
        }
    }

    weMadeSuggestion = true;
    return curSuggestion;
}

void Bot::showCard(Player player, Card card)
{
    std::lock_guard<std::mutex> l(lock);

    LOG_INFO("showed card " + std::string(card) + " from " + playerToStr(player));

    notes[this->player][card].seen = true;
    notes[player][card].has = true;
    if (weMadeSuggestion) {
        if (log.waiting())
            log.clear();
        log.addSuggestion(this->player, curSuggestion);
        log.addShow(player);
        weMadeSuggestion = false;
    }
    notesHook();
}

void Bot::noShowCard()
{
    std::lock_guard<std::mutex> l(lock);

    LOG_INFO("nobody was able to show us a card");

    if (weMadeSuggestion) {
        if (log.waiting())
            log.clear();
        log.addSuggestion(this->player, curSuggestion);
        log.addNoShow();
        notesHook(true);
        weMadeSuggestion = false;
    }
}

Bot::Card Bot::getCard(Player player, std::vector<Card> cards)
{
    std::lock_guard<std::mutex> l(lock);

    std::vector<Bot::Card> ncs;

    for (auto c : cards)
        if (notes[this->player][c].has && !notes[this->player][c].table)
            ncs.push_back(c);

    cards = ncs;

    if (cards.empty())
        throw std::invalid_argument("getCard called with no valid cards");

    auto cs = [&]() {
        std::string s = "[";
        for (auto c : cards)
            s += std::string(c) + ",";
        s.pop_back();
        return s + "]";
    };

    LOG_INFO("asked to pick a card from " + cs());

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

    // we now know the other player has seen this card
    notes[player][cards[min]].seen = true;

    return cards[min];
}

void Bot::newTurn()
{
    std::lock_guard<std::mutex> l(lock);

    LOG_INFO("notified that new turn is starting");

    log.clear();
}

std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>> Bot::getNotes()
{
    std::lock_guard<std::mutex> l(lock);

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
        for (int i = 0; i <= int(MAX_PLAYER); i++) {
            bool found = false;
            for (auto player : order) {
                if (!notes[player][Player(i)].lacks) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                envelope.player = Player(i);
                notes[this->player][envelope.player].envelope = true;
                envelope.havePlayer = true;
                LOG_LOGIC("SOLVED: " + playerToStr(envelope.player) + " is the envelope player (all-lacks)");
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
                notes[this->player][envelope.player].envelope = true;
                envelope.havePlayer = true;
                LOG_LOGIC("SOLVED: " + playerToStr(envelope.player) + " is the envelope player (no-has)");
            }
        }
    }

    if (!envelope.haveWeapon) {
        for (int i = 0; i <= int(MAX_WEAPON); i++) {
            bool found = false;
            for (auto player : order) {
                if (!notes[player][Weapon(i)].lacks) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                envelope.weapon = Weapon(i);
                notes[this->player][envelope.weapon].envelope = true;
                envelope.haveWeapon = true;
                LOG_LOGIC("SOLVED: " + weaponToStr(envelope.weapon) + " is the envelope weapon (all-lacks)");
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
                notes[this->player][envelope.weapon].envelope = true;
                envelope.haveWeapon = true;
                LOG_LOGIC("SOLVED: " + weaponToStr(envelope.weapon) + " is the envelope weapon (no-has)");
            }
        }
    }

    if (!envelope.haveRoom) {
        for (int i = 0; i <= int(MAX_ROOM); i++) {
            bool found = false;
            for (auto player : order) {
                if (!notes[player][Room(i)].lacks) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                envelope.room = Room(i);
                notes[this->player][envelope.room].envelope = true;
                envelope.haveRoom = true;
                LOG_LOGIC("SOLVED: " + roomToStr(envelope.room) + " is the envelope room (all-lacks)");
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
                notes[this->player][envelope.room].envelope = true;
                envelope.haveRoom = true;
                LOG_LOGIC("SOLVED: " + roomToStr(envelope.room) + " is the envelope room (no-has)");
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
        if (notes[player][Player(i)].has && !notes[player][Player(i)].table)
            players.push_back(Player(i));

    if (players.empty() && envelope.havePlayer)
        players.push_back(envelope.player);

    if (players.empty())
        for (int i = 0; i <= int(MAX_PLAYER); i++)
            if (!notes[player][Player(i)].table)
                players.push_back(Player(i));

    return players;
}

std::vector<Bot::Weapon> Bot::getSafeWeapons()
{
    std::vector<Weapon> weapons;

    for (int i = 0; i <= int(MAX_WEAPON); i++)
        if (notes[player][Weapon(i)].has && !notes[player][Weapon(i)].table)
            weapons.push_back(Weapon(i));

    if (weapons.empty() && envelope.haveWeapon)
        weapons.push_back(envelope.weapon);

    if (weapons.empty())
        for (int i = 0; i <= int(MAX_WEAPON); i++)
            if (!notes[player][Weapon(i)].table)
                weapons.push_back(Weapon(i));

    return weapons;
}

std::vector<Bot::Room> Bot::getSafeRooms()
{
    std::vector<Room> rooms;

    for (int i = 0; i <= int(MAX_ROOM); i++)
        if (notes[player][Room(i)].has && !notes[player][Room(i)].table)
            rooms.push_back(Room(i));

    if (rooms.empty() && envelope.haveRoom)
        rooms.push_back(envelope.room);

    if (rooms.empty())
        for (int i = 0; i <= int(MAX_ROOM); i++)
            if (!notes[player][Room(i)].table)
                rooms.push_back(Room(i));

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

    throw std::invalid_argument("pos " + std::to_string(pos) + " is not a valid room position");
}

int Bot::findNextMove(int allowedMoves, std::vector<Room> wanted, bool allowOccupied)
{
    // first check if we are already in a wanted room - if we are, remove it from the wanted list
    if ((board[this->player] != 0) && (board[this->player] < Board::ROOM_COUNT) && contains(wanted,
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
        LOG_LOGIC("unable to reach any wanted room due to blockages");
        // first check if we can move at all
        bool fenced = true;
        for (auto n : start.getNeighbours()) {
            if (occupied[n])
                continue;
            fenced = false;
            break;
        }

        if (fenced) { // we're completely stuck, so just stay here
            LOG_LOGIC("unable to move at all, staying put");
            return board[this->player];
        }

        if (unwantedRooms.empty()) { // we cannot reach anything
            LOG_LOGIC("unable to reach any rooms, moving towards closest room");
            // move towards the closest room
            return findClosestRoom();
        } else { // we can reach at least one unwanted room
            LOG_LOGIC("can reach unwanted room, going towards best one");
            // move towards the unwanted room with the closest wanted room
            return findBestUnwanted();
        }
    } else { // some of the rooms were unblocked
        // first check if we can reach any of the rooms, if we can - go there
        for (auto d : dists) {
            if (d.second <= allowedMoves) {
                LOG_LOGIC("can reach a wanted room, going there");
                return getRoomPos(d.first);
            }
        }

        // if there is any unwanted rooms we can go in to, rather go there
        if (!unwantedRooms.empty()) {
            LOG_LOGIC("can reach unwanted room, going towards best one");
            return findBestUnwanted();
        }

        // no rooms were found that was in reach, go towards the closest room
        LOG_LOGIC("unable to reach any rooms, moving towards closest room");
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

Bot::Card::Type Bot::findLeastKnown()
{
    int playerCount = 0;
    int weaponCount = 0;
    int roomCount = 0;

    for (auto player : order) {
        for (int i = 0; i <= int(MAX_PLAYER); i++)
            if (notes[player][Player(i)].has)
                playerCount++;

        for (int i = 0; i <= int(MAX_WEAPON); i++)
            if (notes[player][Weapon(i)].has)
                weaponCount++;

        for (int i = 0; i <= int(MAX_ROOM); i++)
            if (notes[player][Room(i)].has)
                roomCount++;
    }

    playerCount = int(MAX_PLAYER) - playerCount;
    weaponCount = int(MAX_WEAPON) - weaponCount;
    roomCount = int(MAX_ROOM) - roomCount;

    if (playerCount > weaponCount) {
        if (playerCount > roomCount)
            return Card::PLAYER;
        else
            return Card::ROOM;
    } else {
        if (weaponCount > roomCount)
            return Card::WEAPON;
        else
            return Card::ROOM;
    }
}

// vim: set expandtab textwidth=100:
