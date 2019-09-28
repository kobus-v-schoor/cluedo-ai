/**
 * \file bot.cpp
 * \author Kobus van Schoor
 */

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
    order(order)
{
    // creates a notes entry for every player
    for (auto o : order)
        notes[o];

    // sets all cards as lacking for this player
    for (int i = 0; i <= int(WHITE); i++)
        notes[player][Player(i)].lacks = true;
    for (int i = 0; i <= int(SPANNER); i++)
        notes[player][Weapon(i)].lacks = true;
    for (int i = 0; i <= int(GAMES_ROOM); i++)
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
    std::vector<bool> occupied(Board::BOARD_SIZE, false);

    for (auto p : board)
        occupied[p.second] = true;

    if (!envelope.haveRoom) {
    } else if (!envelope.havePlayer) {
    } else if (!envelope.haveWeapon) {
    } else {
        Position dest(0);
        Position cur(board[player]);

        return cur.path(dest, occupied, 1).partial(allowedMoves, occupied);
    }

    return 0;
}

Bot::Suggestion Bot::getSuggestion(){ return Suggestion(Player(0),Weapon(0),Room(0)); }

void Bot::showCard(Player player, Card card)
{
    notes[this->player][card].seen = true;
    notes[player][card].has = true;
    notesHook();
}

void Bot::noShowCard()
{
    //runDeductors();
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

    if (!envelope.havePlayer) {
        for (int i = 0; (i <= int(Player::WHITE)); i++) {
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
                LOG_LOGIC("Deduced that " << envelope.player << " is the player in the envelope");
                break;
            }
        }
    }

    if (!envelope.haveWeapon) {
        for (int i = 0; (i <= int(Weapon::SPANNER)); i++) {
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
                LOG_LOGIC("Deduced that " << envelope.weapon << " is the weapon in the envelope");
                break;
            }
        }
    }

    if (!envelope.haveRoom) {
        for (int i = 0; (i <= int(Room::GAMES_ROOM)); i++) {
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
                LOG_LOGIC("Deduced that " << envelope.room << " is the room in the envelope");
                break;
            }
        }
    }

    return env != envelope;
}

Deck Bot::getWantedDeck()
{
    Deck deck;

    if (!envelope.havePlayer) {
        for (int i = 0; i <= int(Player::WHITE); i++) {
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
        for (int i = 0; i <= int(Weapon::SPANNER); i++) {
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
        for (int i = 0; i <= int(Room::GAMES_ROOM); i++) {
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
        suggestion.room;
    return ostream;
}

// vim: set expandtab textwidth=100:
