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
#include <vector>

#include "lockable.h"
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
		/// Various mutexes that keep this object thread-safe.
		enum Lock { JoinMutex };

	public:
		/**
		 * Creates an empty room with the given id number and owner.
		 *
		 * @param gid The room id number.
		 * @param The room owner.
		 */
		Room(int gid, const std::string &owner);

		/**
		 * Assigns computer-controlled players to empty positions.
		 */
		void assignAI();

		/**
		 * Generates a random turn order for all players.
		 */
		void randomizeTurnOrder();

		/**
		 * Asks each player to choose a game token.
		 * Players choose pieces based on turn order. Computer players will randomly
		 * choose a token.
		 */
		void tokenSelection();

		/**
		 * Causes the current thread to sleep until someone joins the room.
		 * After this method returns, the JoinMutex mutex will be locked -- be sure
		 * to unlock it when you're done to avoid deadlocks!
		 */
		void waitOnJoin();

		/**
		 * Causes the current thread to awake when waiting for players to join.
		 * Note that the aforementioned JoinMutex mutex will now be locked.
		 */
		void awakeOnJoin();

		/**
		 * Locks the defined room mutex.
		 *
		 * @param lock Which mutex to lock.
		 */
		void lock(const Room::Lock &lock);

		/**
		 * Unlocks the defined room mutex.
		 *
		 * @param lock Which mutex to unlock.
		 */
		void unlock(const Room::Lock &lock);

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
		 * Finds the index for the given username.
		 *
		 * @return The player's index, or -1 if no such player is in the room.
		 */
		int getPlayerIndex(const std::string &username);

		/**
		 * Adds a player to the game room.
		 *
		 * @param player The player to add.
		 */
		void addPlayer(Player *player);

		/**
		 * Removes a player from the game room.
		 *
		 * @param player The player to remove.
		 * @return The index the player was previously assigned.
		 */
		int removePlayer(const Player *player);

		/**
		 * Returns a list of players in this room.
		 * Do not try to check the amount of players in the room by calling the vector's
		 * size() method. The vector is preallocated to have space for four (4) pointers,
		 * so this will always lead to unexpected results in code. Use the getNumPlayers()
		 * method instead.
		 *
		 * @return A vector of usernames.
		 */
		std::vector<Player*> getPlayers();

		/**
		 * Returns the amount of human players (accepted and not accepted) in this room.
		 *
		 * @return The number of human players.
		 */
		int getNumPlayers();

	private:
		/// The id number of the room.
		int m_Gid;

		/// The owner of this room.
		std::string m_Owner;

		/// The rules of this room.
		Rules m_Rules;

		/// List of players in the room.
		std::vector<Player*> m_Players;

		/// Indicies into the m_Players vector, based on turn order.
		std::vector<int> m_TurnOrder;

		/// Mutex that locks the thread when a player is joining.
		pthread_mutex_t m_JoinMutex;

		/// Condition variable for the ownerJoinMutex object.
		pthread_cond_t m_OwnerJoinCond;
};

#endif
