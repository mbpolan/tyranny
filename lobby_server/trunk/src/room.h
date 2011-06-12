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

class Room {
	public:
		/// Types of game rooms.
		enum Type { Public, Private };

		/// Room statuses.
		enum Status { Open, InProgress, Closed };

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
		/**
		 * Creates an empty room with the given id number and owner.
		 *
		 * @param gid The room id number.
		 * @param The room owner.
		 * @param type What kind of room this is.
		 * @param password The room password, or an empty string if none.
		 * @param friendsOnly Whether or not only friends of the owner may join.
		 */
		Room(int gid, const std::string &owner, const Type &type, const std::string &password, bool friendsOnly);

		/**
		 * Sets the rules for this room.
		 *
		 * @param rules A Rules object prepared for this room.
		 */
		void setRules(const Rules &rules) { m_Rules=rules; }

		/**
		 * Returns the rules for this room.
		 *
		 * @return A Rules object.
		 */
		Rules getRules() const { return m_Rules; }

		/**
		 * Sets the connection details for hosting this room.
		 *
		 * @param host The hostname/IP address of the hosting game server.
		 * @param port The port of the hosting game server.
		 */
		void setConnectionInfo(const std::string &host, int port);

		/**
		 * Returns the connection details of the game server hosting this room.
		 *
		 * @param host Sets this value to the hostname/IP address.
		 * @param port Sets this value to the port.
		 */
		void getConnectionInfo(std::string &host, int &port);

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
		 * Returns the room's type.
		 *
		 * @return The room's type.
		 */
		Type getType() const { return m_Type; }

		/**
		 * Returns the room's password.
		 *
		 * @return The password for this room.
		 */
		std::string getPassword() const { return m_Password; }

		/**
		 * Returns whether or not this room is for the friends of the owner only.
		 *
		 * @return true if yes, false otherwise.
		 */
		bool isFriendsOnly() const { return m_FriendsOnly; }

		/**
		 * Sets the room's status.
		 *
		 * @param status The status of the room at this point.
		 */
		void setStatus(const Status &status) { m_Status=status; }

		/**
		 * Returns the room's status.
		 *
		 * @return The status of the room at this point.
		 */
		Status getStatus() const { return m_Status; }

		/**
		 * Adds a player to the game room.
		 *
		 * @param username The user to add.
		 */
		void addPlayer(const std::string &username) { m_Players.push_back(username); }

		/**
		 * Removes a player from the game room.
		 *
		 * @param username The user to remove.
		 */
		void removePlayer(const std::string &username);

		/**
		 * Returns a list of players in this room.
		 *
		 * @return A vector of usernames.
		 */
		std::vector<std::string> getPlayers() const { return m_Players; }

	private:
		/// The id number of the room.
		int m_Gid;

		/// The type of room.
		Type m_Type;

		/// The owner of this room.
		std::string m_Owner;

		/// The status of the room.
		Status m_Status;

		/// The rules of this room.
		Rules m_Rules;

		/// The hostname of the hosting server.
		std::string m_Host;

		/// The port of the hosting server.
		int m_Port;

		/// The room password, if any.
		std::string m_Password;

		/// Whether or not the room is for owner friends only.
		bool m_FriendsOnly;

		/// List of players in the room.
		std::vector<std::string> m_Players;
};

#endif
