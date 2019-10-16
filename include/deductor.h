/**
 * \file deductor.h
 * \author Kobus van Schoor
 */

#pragma once

#include "bot.h"
#include <map>

namespace AI {
    class Deductor {
        public:
            Deductor(){}
            virtual ~Deductor(){}

            /**
             * \brief Attemps to make a deduction
             * \returns true if a deduction was made
             */
            virtual bool run(Bot::SuggestionLog log, std::map<Bot::Player, std::map<Bot::Card, Bot::Notes>>& notes) =0;

        protected:
            Bot::Player player;
    };
}

// vim: set expandtab textwidth=100:
