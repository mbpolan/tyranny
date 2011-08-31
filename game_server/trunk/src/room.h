/***************************************************************************
 *   Copyright (C) 2011 by the Tyranny Development Team                    *
 *   http://tyranny.sf.net                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
// room.h: definition of the Room class.

#ifndef ROOM_H
#define ROOM_H

#include <iostream>
#include <map>
#include <queue>
#include <vector>

#include "fdbuffer.h"
#include "lockable.h"
#include "human.h"
#include "player.h"

/**
 * Model class for game rooms.
 */
class Room: public Lockable {
	public:
		/**
		 * Defines the rules set by the room owner.
		 */
		class Rules {
			public:
				/// Property redistribution method.
				enum RedistMethod { RandomToPlayers=0, ReturnToBank };

			public:
				/// Default constructor
				Rules() { }

				/**
				 * Constructs a rule set object with the given user-defined rules.
				 *
				 * @param maxTurns The maximum amount of turns.
				 * @param maxHumans The maximum amount of human players.
				 * @param freeParkReward Money paid for stepping on Free Parking.
				 * @param incomeTaxChoice Whether or not players can choose what to pay on Income Tax.
				 * @param rmethod The property redistribution method.
				 */
				Rules(int maxTurns, int maxHumans, int freeParkReward, bool incomeTaxChoice, const RedistMethod &rmethod) {
					m_MaxTurns=maxTurns;
					m_MaxHumans=maxHumans;
					m_FreeParkReward=freeParkReward;
					m_ITChoice=incomeTaxChoice;
					m_PropRedist=rmethod;
				}

				/**
				 * Returns the maximum amount of game turns.
				 * @return Max game turns.
				 */
				int getMaxTurns() const { return m_MaxTurns; }

				/**
				 * Returns the maximum amount of human players.
				 * @return Max human players.
				 */
				int getMaxHumans() const { return m_MaxHumans; }

				/**
				 * Returns the money paid for stepping on Free Parking.
				 * @return The money for the bank to pay.
				 */
				int getFreeParkReward() const { return m_FreeParkReward; }

				/**
				 * Returns whether or not players have a choice of fee for Income Tax.
				 * @return true if yes, false otherwise.
				 */
				int getIncomeTaxChoice() const { return m_ITChoice; }

				/**
				 * Returns the property redistribution method.
				 * @return The method in which properties are distributed after bankruptcy.
				 */
				RedistMethod getRedistributionMethod() const { return m_PropRedist; }

			private:
				int m_MaxTurns;
				int m_MaxHumans;
				int m_FreeParkReward;
				bool m_ITChoice;
				RedistMethod m_PropRedist;
		};

	public:
		/// Phases in the room's lifespan.
		enum Phase { Init=0, AwaitOwner, AwaitMorePlayers, FindTurnOrder, TokenSelection, Terminating };

	public:
		/**
		 * Creates an empty room with the given id number and owner.
		 *
		 * @param gid The room id number.
		 * @param The room owner.
		 */
		Room(int gid, const std::string &owner);

		/**
		 * Sets the rules for this room.
		 *
		 * @param rules A Rules object prepared for this room.
		 */
		void setRules(const Rules &rules);

		/**
		 * Returns the rules for this room.
		 *
		 * @return A Rules object.
		 */
		Rules getRules();

		/**
		 * Sets this room's current phase.
		 *
		 * @param phase The phase this room is now in.
		 */
		void setPhase(const Phase &phase);

		/**
		 * Returns this room's current phase.
		 *
		 * @return This room's current phase.
		 */
		Phase getPhase();

		/**
		 * Returns the room's id number.
		 *
		 * @return The id number of the room.
		 */
		int getGid() const { return m_Gid; }

		/**
		 * Returns the room's owner.
		 *
		 * @return The username of the room owner.
		 */
		std::string getOwner() const { return m_Owner; }

		/**
		 * Determines if a new player can join this room.
		 *
		 * @param result This gets set to a result message if the return value is false.
		 * @return true if yes, false if no.
		 */
		bool allowNewPlayers(std::string &result);

		/**
		 * Adds the owner as a joined player.
		 * Call this method only once: when the room is awaiting an owner, a call
		 * to this method flags the game to "begin." Subsequent calls may corrupt
		 * the state of the room (to be fixed in the future).
		 *
		 * @param player The owner player object.
		 */
		void addOwner(Player *player);

		/**
		 * Adds a given player to the internal room queue.
		 * When the room cycles, it will add the player to the game.
		 *
		 * @param player The player to add.
		 */
		void queuePlayer(Player *player);

		/**
		 * Starts the procedure to play the game in the room.
		 */
		void begin();

	private:
		/**
		 * Removes a given player from the room.
		 * This method also notifies all clients of the disconnected player,
		 * be it a computer or another human player. If so requested, the
		 * player can be replaced by a computer controlled one.
		 *
		 * @param player The player to remove.
		 * @param replace true to replace with a computer player, false otherwise.
		 */
		void removePlayer(Player *player, bool replace);

		/**
		 * Maps a vector of socket file descriptors to the human players who
		 * are connected through them.
		 *
		 * @param fds A vector of file descriptors.
		 * @return A translated vector of matching players.
		 */
		std::vector<Human*> mapToPlayers(const std::vector<int> &fds);

		/**
		 * Alerts all connected clients that a player (computer or human) has joined
		 * the game room.
		 *
		 * @param player The player who joined.
		 * @param index The joining player's assigned index.
		 */
		void broadcastPlayerJoined(Player *player, int index);

		/**
		 * Alerts all clients of the chosen turn order.
		 */
		void broadcastTurnOrder();

		/**
		 * Sends all clients the given protocol notification.
		 *
		 * @param note The notification to send.
		 */
		void broadcastNotification(const Protocol::Notification &note);

		/**
		 * Alerts all clients that the given player has chosen a token.
		 *
		 * @param player Index of the player who chosen the token.
		 * @param piece The index of the token (range is [0,5]).
		 */
		void broadcastTokenChosen(int player, int piece);

		/// Assigns computer players to empty slots.
		void assignAI();

		/// Removes any disconnected or dead clients.
		void handleDisconnectedPlayers();

		/// Tries to add waiting players to the game room player list.
		void handleQueuedPlayers();

		/**
		 * Handles general player actions during the pre-game phase.
		 */
		void handleAwaitMorePlayers(const FDBuffer::WaitCode &fdc);

		/**
		 * Handles assigning turn orders to players.
		 */
		void handleFindTurnOrder(const FDBuffer::WaitCode &fdc);

		/**
		 * Handles dealing with the token selection process.
		 */
		void handleTokenSelection(const FDBuffer::WaitCode &fdc);

		/// The id number of the room.
		int m_Gid;

		/// The owner of this room.
		std::string m_Owner;

		/// The rules of this room.
		Rules m_Rules;

		/// The current room phase.
		Phase m_Phase;

		/// The number of human players.
		int m_NumHumans;

		/// Marker for the current player who has control.
		int m_CurPlayer;

		/**
		 * Predetermined turn order for players.
		 * The indices of the vector map into the m_Players vector,
		 * in such a manner:
		 *
		 * m_TurnOrder[0] -> 1 (player 2 goes first)
		 * m_TurnOrder[1] -> 0 (player 1 goes second)
		 * m_TurnOrder[2] -> 3 (player 4 goes third)
		 * m_TurnOrder[3] -> 2 (player 3 goes last)
		 */
		std::vector<int> m_TurnOrder;

		/**
		 * Vector of chosen pieces for players.
		 * Each index into the vector maps into the m_Players vector. The
		 * range is [0,5].
		 */
		std::vector<int> m_ChosenPieces;

		/// The players (both human and computer) in this room.
		std::vector<Player*> m_Players;

		/// Players waiting to be added to the game.
		std::queue<Player*> m_Queued;

		/// Temporary vector for active player sockets.
		std::vector<Human*> m_ActiveSockets;

		/// A controller for connected clients.
		FDBuffer m_FDBuffer;
};

#endif
