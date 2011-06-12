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
// usermanager.h: definition of the UserManager class.

#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <iostream>
#include <map>
#include <pthread.h>

#include "room.h"
#include "user.h"

class UserManager {
	public:
		/// Determines a user's activity.
		enum UserActivity { RoomOwner, Participant, Idle };

	public:
		/// Default constructor.
		UserManager();

		/**
		 * Cleans up data associated with this management pool.
		 * Note that no User object pointers are destroyed by this method.
		 */
		~UserManager();

		/**
		 * Returns a pointer to the global user manager.
		 *
		 * @return A pointer to a UserManager object.
		 */
		static UserManager* instance();

		/**
		 * Adds a user to the management pool.
		 *
		 * @param user The user to add.
		 */
		void addUser(User *user);

		/**
		 * Removes a user from the management pool.
		 *
		 * @param user The user to remove.
		 */
		void removeUser(User *user);

		/**
		 * Determines if a user is the owner of a room, a participant, or idle.
		 *
		 * @param username The user to test.
		 * @return An activity code.
		 */
		UserActivity isUserActive(const std::string &username);

		/**
		 * Sends a chat message to all clients from another user.
		 *
		 * @param user The user who sent the message.
		 * @param message The contents of the message.
		 */
		void broadcastChatMessage(const std::string &user, const std::string &message);

		/**
		 * Sends the target user a status update about another use.
		 *
		 * @param user The user to whom a status update should be sent to.
		 * @param target The user whose status we are concerned about.
		 * @param online True if the target is online, false otherwise.
		 */
		void sendUserStatusUpdate(const std::string &user, const std::string &target, bool online);

		/**
		 * Sends the target user a list of current rooms.
		 *
		 * @param user The user to whom the list should be sent.
		 */
		void sendRoomList(const std::string &user);

		/**
		 * Opens a new game room with the given parameters, and sends an update to all clients.
		 *
		 * @param owner The room's owner.
		 * @param password The room's password.
		 * @param friendsOnly Whether or not only friends of the owner may join.
		 * @param rules The rules for this room.
		 * @param host The hostname/IP address of the game server hosting this room.
		 * @param port The port of the hosting game server.
		 * @return The assigned id number for this room
		 */
		int registerGameRoom(const std::string &owner, const std::string &password, bool friendsOnly,
							 const Room::Rules &rules, const std::string &host, int port);

		/**
		 * Attempts to join the given user to the game room with id number gid.
		 * In the case that this method fails, due to whatever reason, the parameters host and port will
		 * not be set to anything, and instead, only error will be set to a string value describing
		 * what went wrong. Otherwise, only the host and port arguments will be set on success.
		 *
		 * @param gid The room id number.
		 * @param username The user who wishes to join.
		 * @param password The room password.
		 * @param host This gets set to the hosting game server's hostname/IP address.
		 * @param port This gets set to the hosting game server's port.
		 * @param error This gets set to a description of an error if this method fails.
		 * @return true if the user joined the room, false otherwise.
		 */
		bool joinGameRoom(int gid, const std::string &username, const std::string &password,
						  std::string &host, int &port, std::string &error);

		/**
		 * An overloaded method for convenience. Joins a player to a game room, not returning
		 * details about the game server hosting the room.
		 *
		 * @param gid The room id number.
		 * @param username The user who wishes to join.
		 * @param password The room password.
		 * @param error This gets set to a description of an error if this method fails.
		 * @return true if the user joined the room, false otherwise.
		 */
		bool joinGameRoom(int gid, const std::string &username, const std::string &password, std::string &error);

	private:
		/**
		 * Finds the specified user and returns a pointer to his object if he's online.
		 *
		 * @param username The user to find.
		 * @return A pointer to a User object, or NULL if the user is not online.
		 */
		User* getOnlineUser(const std::string &username) const;

		/// Map of users, hashed according to their usernames.
		std::map<std::string, User*> m_UserMap;

		/// Map of current rooms, hashed according to their id numbers.
		std::map<int, Room*> m_Rooms;

		/// Synchronization variables.
		pthread_mutex_t m_Mutex;
};

#endif
