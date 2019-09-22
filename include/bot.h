/**
 * \file bot.h
 * \author Kobus van Schoor
 */

#pragma once
#include "macros.h"
#include "position.h"
#include <vector>
#include <utility>

namespace AI {
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

                const Player player;
                const Weapon weapon;
                const Room room;
            };

            /**
             * \brief Bot class constructor
             * \param player The board character the AI will be playing as
             */
            NOT_IMPLEMENTED
            Bot(Player player);

            /**
             * \brief Set the cards dealt to the player
             *
             * This will be used in the beginning of the game when the player gets dealt various
             * cards.  This can also be used when the AI takes over from a human player to inform
             * the AI player of all the cards the human player has been shown during the game,
             */
            NOT_IMPLEMENTED
            void setCards(std::vector<Card> cards);

            /**
             * \brief This can be used to update all of the player's positions on the board
             *
             * This should be used when the AI gets initialized either at the beginning of the game
             * or when an AI player replaces a human player
             */
            NOT_IMPLEMENTED
            void updateBoard(std::vector<std::pair<Player, Position>> players);

            /**
             * \brief Used to indicate that a player has moved their position
             *
             * This will update the internal board state to match the player's new position
             */
            NOT_IMPLEMENTED
            void movePlayer(Player player, Position position);

            /**
             * \brief Used to inform AI that another player has made a suggestion.
             *
             * \param player The player who made the suggestion
             * \param suggestion The suggestion the player made
             */
            NOT_IMPLEMENTED
            void madeSuggestion(Player player, Suggestion suggestion);

            /**
             * \brief Used to notify AI that a player was shown a card by another player
             *
             * This should be used to notify the AI that one player was able to show another player
             * a card after the other player asked about a card.
             *
             * \param asked The player that asked the question/made a suggestion
             * \param showed The player that showed the other player a card
             */
            NOT_IMPLEMENTED
            void shownCard(Player asked, Player showed);

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
             * \brief Used to request a card to show to another player that made a suggestion
             *
             * \param cards The cards that the AI has to choose from as specified by the server
             *
             * \returns the card to show to the other player
             */
            NOT_IMPLEMENTED
            Card getCard(std::vector<Card> cards);
    };
}

// vim: set expandtab textwidth=100:
