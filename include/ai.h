#pragma once
#include <vector>
#include <utility>

/**
 * \brief The main AI class - clients and servers will spawn this class.
 * \author Kobus van Schoor
 */

class AI {
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
         * This class will be used for any parameters that can be of any card type. Using the type
         * member can be used to cast card back to original type
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
         * \brief Abstraction of player positions
         *
         * This will be used whenever a positions needs to be specified. This class will then be
         * used to calculate a position useable by the internals of the AI.
         *
         * \param pos This is the integer position used in the board layout
         */
        struct Position {
            Position(int pos);
        };

        /**
         * \brief AI class constructor
         * \param player The player the AI will be playing as
         */
        AI(Player player);

        /**
         * \brief Set the cards dealt to the player
         *
         * This will be used in the beginning of the game when the player gets dealt various cards
         */
        void setCards(std::vector<Card> cards);

        /**
         * \brief This can be used to update all the player's positions on the board
         *
         * This should be used when the AI gets initialized either at the beginning of the game or
         * when an AI player replaces a human player
         */
        void updateBoard(std::vector<std::pair<Player, Position>> players);
};

// vim: set expandtab textwidth=100:
