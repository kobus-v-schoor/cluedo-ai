/**
 * \file bot.cpp
 * \author Kobus van Schoor
 */

#include "../include/bot.h"

// deductors
#include "../include/deductor.h"
#include "../include/deductors/local-exclude.h"

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
    return dealt || seen || shown || lacks || deduced;
}

bool Bot::Notes::conclusion()
{
    if ((dealt || seen || shown || deduced) != !lacks)
        throw std::logic_error("conflicting deductions has been set for note");
    return !lacks;
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

Bot::Bot(const Player player, std::vector<Player> order) :
    player(player),
    order(order)
{
    deductors.push_back(new LocalExcludeDeductor());
}

Bot::~Bot()
{
    for (auto d : deductors)
        delete d;
}

void Bot::setCards(const std::vector<Card> cards)
{
    for (auto c : cards)
        notes[player][c].dealt = true;
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
    //runDeductors();
}

void Bot::madeSuggestion(Player player, Suggestion suggestion, bool accuse)
{
    log.addSuggestion(player, suggestion);
}

void Bot::shownCard(Player showed)
{
    log.addShow(showed);
    runDeductors();
}

void Bot::noOtherShownCard()
{
    log.addNoShow();
    runDeductors();
}

int Bot::getMove(int allowedMoves){ return 0; }

Bot::Suggestion Bot::getSuggestion(){ return Suggestion(Player(0),Weapon(0),Room(0)); }

void Bot::showCard(Player player, Card card)
{
    notes[this->player][card].seen = true;
    notes[player][card].dealt = true;
    notes[player][card].shown = true;
    runDeductors();
}

void Bot::noShowCard()
{
    //runDeductors();
}

Bot::Card Bot::getCard(std::vector<Card> cards){ return Card(Player(0)); }

void Bot::newTurn()
{
    log.clear();
}

void Bot::runDeductors()
{
    bool made;
    int count = 0;
    do {
        made = false;
        for (auto d : deductors) {
            if (d->run(log, notes)) {
                made = true;
                break;
            }
        }
        count++;
    } while (made && (count < MAX_DEDUCTOR_RUN_COUNT));
}

// vim: set expandtab textwidth=100:
