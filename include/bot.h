/**
 * \file bot.h
 * \author Kobus van Schoor
 */

#pragma once
#include "macros.h"
#include "position.h"
#include <vector>
#include <utility>
#include <map>
#include <ostream>

namespace AI {
    /**
     * \brief Maximum amount of times the deductor loop can run
     *
     * This is added as a safety feature to prevent infinite loops should something go wrong in one
     * of the deductors
     */
    const int MAX_DEDUCTOR_RUN_COUNT = 100;

    class Deductor;

    /**
     * \brief The main AI class - clients and servers will spawn and use only this class.
     *
     * This class can be used to replace a human player in the game of Cluedo. Please note that this
     * class is stateful, meaning that the instance that you create of this class should be used for
     * the entirety of the game as the AI will store and learn from data passed to it as the game
     * progresses (will not just use the current state of the board).
     */
    class Bot {
        public:
            /** Used to identify players */
            enum Player {
                SCARLET,
                PLUM,
                PEACOCK,
                GREEN,
                MUSTARD,
                WHITE
            };

            /** Used to identify weapons */
            enum Weapon {
                CANDLESTICK,
                KNIFE,
                LEAD_PIPE,
                REVOLVER,
                ROPE,
                SPANNER
            };

            /** Used to identify rooms */
            enum Room {
                BEDROOM,
                BATHROOM,
                STUDY,
                KITCHEN,
                DINING_ROOM,
                LIVING_ROOM,
                COURTYARD,
                GARAGE,
                GAMES_ROOM
            };

            /**
             * \brief Class used to encapsulate a generic card
             *
             * This class will be used for any parameters that can be of any card type. Using the
             * type member can be used to cast the card back to its original type
             */
            struct Card {
                Card(Player p);
                Card(Weapon w);
                Card(Room r);

                enum Type {
                    PLAYER,
                    WEAPON,
                    ROOM
                };

                /**
                 * \brief The int value of the original enum
                 *
                 * Use the type member to recast the int back to its original value
                 */
                const int card;
                const Type type;

                bool operator<(const Card& other) const;
                bool operator==(const Card& other) const;
            };

            /**
             * \brief Collection of cards to form a suggestion or accusation
             */
            struct Suggestion {
                /**
                 * \param p The player that was accused by the suggestion
                 * \param w The weapon that the player was accused with
                 * \param r The room that the player was accused of committing the murder in
                 */
                Suggestion(Player p, Weapon w, Room r);

                bool operator==(const Suggestion& other);

                Player player;
                Weapon weapon;
                Room room;
            };

            /**
             * \brief Used to mark various attributes for cards (the "notes" the player is making)
             */
            struct Notes {
                /**
                 * \brief Player was dealt the card during game init
                 */
                bool dealt = false;

                /**
                 * \brief Player has "seen" the card i.e. the player has seen the card because
                 * another player showed it to them
                 */
                bool seen = false;

                /**
                 * \brief Player has shown the card to someone else
                 */
                bool shown = false;

                /**
                 * \brief Player does not have the card
                 */
                bool lacks = false;

                /**
                 * \brief Deduced that the player has the card
                 */
                bool deduced = false;

                /**
                 * \returns true if a conclusion can be made about a player and a card
                 */
                bool concluded();

                /**
                 * \returns true if a player has or has seen a card
                 * \note first check that a conclusion has been drawn by calling concluded()
                 * \throw std::logic_error if conflicting attributes has been set
                 */
                bool conclusion();
            };

            /**
             * \brief This gets used in the SuggestionLog() class
             */
            struct SuggestionLogItem {
                /**
                 * \brief The suggestion the player made
                 */
                Suggestion suggestion = Suggestion(Player(0), Weapon(0), Room(0));

                /**
                 * \brief The player who made the suggestion
                 */
                Player from;

                /**
                 * \brief If a player showed a card, the player who showed the card
                 */
                Player show;

                /**
                 * \brief True if a player showed a card
                 */
                bool showed;
            };

            /**
             * \brief abstracts suggestion logging.
             *
             * This is done because the madeSuggestion() and shownCard() function are called
             * separately. If the two aren't correctly used in conjunction with each other the
             * integrity of the log could be at risk. This class only adds entries if they are
             * correctly logged and hence insures integrity of the log.
             */
            class SuggestionLog {
                public:
                    /**
                     * \brief Adds a suggestion to the log
                     * \throw std::runtime_error if trying to add a suggestion while waiting
                     */
                    void addSuggestion(Player from, Suggestion sug);

                    /**
                     * \brief Adds a player that showed a card
                     * \throw std::runtime_error if not waiting for player
                     */
                    void addShow(Player player);

                    /**
                     * \brief Adds a no-show to the log
                     * \throw std::runtime_error if not waiting for player
                     */
                    void addNoShow();

                    /**
                     * \returns true if waiting for player
                     */
                    bool waiting();

                    /**
                     * \brief Stops the log from waiting (useful on next turn
                     */
                    void clear();

                    /**
                     * \returns the log
                     */
                    std::vector<SuggestionLogItem> log();

                private:
                    bool waitingForShow = false;
                    SuggestionLogItem staging;
                    std::vector<SuggestionLogItem> _log;
            };

            /**
             * \brief Bot class constructor
             * \param player The board character the AI will be playing as
             * \param order The order in which players will play, with the first element being the
             * first player.
             */
            Bot(Player player, std::vector<Player> order);

            ~Bot();

            /**
             * \brief Set the cards dealt to the player
             *
             * This will be used in the beginning of the game when the player gets dealt various
             * cards.  This can also be used when the AI takes over from a human player to inform
             * the AI player of all the cards the human player has been shown during the game,
             */
            void setCards(std::vector<Card> cards);

            /**
             * \brief This can be used to update all of the player's positions on the board
             *
             * This should be used when the AI gets initialized either at the beginning of the game
             * or when an AI player replaces a human player
             */
            void updateBoard(std::vector<std::pair<Player, Position>> players);

            /**
             * \brief Used to indicate that a player has moved their position
             *
             * This will update the internal board state to match the player's new position
             */
            void movePlayer(Player player, Position position);

            /**
             * \brief Used to inform AI that another player has made a suggestion.
             *
             * \param player The player who made the suggestion
             * \param suggestion The suggestion the player made
             * \param accuse Set to true if the suggestion was a wrong accusation
             * \note Remember to call the shownCard() or noOtherShownCard() function after using
             * this function to notify the AI if another player was able to show a card!
             */
            void madeSuggestion(Player player, Suggestion suggestion, bool accuse = false);

            /**
             * \brief Used to notify AI that a player was shown a card by another player
             *
             * This should be used to notify the AI that one player was able to show another player
             * a card after the other player asked about a card.
             *
             * \param showed The player that showed the other player a card
             * \note Remember to call this or noOtherShownCard() after calling madeSuggestion()!
             */
            void shownCard(Player showed);

            /**
             * \brief Used to notify the AI that no other player was able to show a card for another
             * player that made a suggestion
             * \note Remember to call this or shownCard() after calling madeSuggestion()!
             */
            void noOtherShownCard();

            /**
             * \brief Used to request a position where AI wants to be moved to
             *
             * This should be used to request the next position where the AI wants to be moved to.
             * The returned position will be a valid position where the player can move to.
             *
             * \param allowedMoves the amount of moves that the AI can make (i.e. dice roll count)
             * \returns the position where the AI wants to move
             */
            NOT_IMPLEMENTED
            int getMove(int allowedMoves);

            /**
             * \brief Used to request a suggestion from the AI.
             *
             * This can be used to get a suggestion from the AI. If the AI is currently located in
             * the center room it can be assumed that the suggestion is in fact an accusation.
             *
             * \returns the suggestion made by the AI. An accusation if the AI is in the center room.
             */
            NOT_IMPLEMENTED
            Suggestion getSuggestion();

            /**
             * \brief Used to show the player a card if another player was able to show a card when
             * the bot made a suggestion
             * \param player the player that showed the card
             * \param card the card that was shown
             */
            void showCard(Player player, Card card);

            /**
             * \brief Used to tell the bot that no player was able to show a card after a suggestion
             * has been made
             */
            NOT_IMPLEMENTED
            void noShowCard();

            /**
             * \brief Used to request a card to show to another player that made a suggestion
             *
             * \param cards The cards that the AI has to choose from as specified by the server
             *
             * \returns the card to show to the other player
             */
            NOT_IMPLEMENTED
            Card getCard(std::vector<Card> cards);

            /**
             * \brief Used to notify the AI that a new turn has just started.
             */
            void newTurn();

        // using protected so that unit-tests class can change access specification
        protected:
            /**
             * \brief runs through all the deductors to make new deductions
             */
            void runDeductors();

            /**
             * \brief The player this bot is playing as
             */
            Player player;

            /**
             * \brief The order in which the players get to play
             */
            std::vector<Player> order;

            /**
             * \brief Holds the notes for all the cards
             */
            std::map<Player, std::map<Card, Notes>> notes;

            /**
             * \brief Holds all the positions of all the players on the board
             */
            std::map<Player, int> board;

            /**
             * \brief Records all the suggestions along with whether somebody showed a card
             */
            SuggestionLog log;

            /**
             * \brief This will hold pointers to instances of every deductor type
             * \note needs to be deleted in destructor
             */
            std::vector<Deductor*> deductors;
    };
}

std::ostream& operator<<(std::ostream& ostream, const AI::Bot::Player player);
std::ostream& operator<<(std::ostream& ostream, const AI::Bot::Weapon weapon);
std::ostream& operator<<(std::ostream& ostream, const AI::Bot::Room room);
std::ostream& operator<<(std::ostream& ostream, const AI::Bot::Suggestion suggestion);

// vim: set expandtab textwidth=100:
