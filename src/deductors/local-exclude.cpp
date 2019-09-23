/**
 * \file local-exclude.cpp
 * \author Kobus van Schoor
 */

#include "../../include/deductors/local-exclude.h"

using namespace AI;

bool LocalExcludeDeductor::run(Bot::SuggestionLog log, std::map<Bot::Player,
        std::map<Bot::Card, Bot::Notes>>& notes)
{
    return false;
}

// vim: set expandtab textwidth=100:
