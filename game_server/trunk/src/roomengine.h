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
// roomengine.h: definition of the RoomEngine class.

#ifndef ROOMENGINE_H
#define ROOMENGINE_H

#include <iostream>
#include <map>

#include "human.h"
#include "lockable.h"
#include "room.h"

/**
 * The core of the game server.
 * This class is responsible for managing the game server itself; it handles
 * accepting or rejecting client connections, room threads, user threads, and
 * the overall mechanics of each room game.
 */
class RoomEngine: public Lockable {
	public:
		/// Creates a game engine with no initial rooms.
		RoomEngine();

		/**
		 * Returns the first instance of this class.
		 *
		 * @return A pointer to a RoomEngine object.
		 */
		static RoomEngine* instance();

		/**
		 * Routine for handling and managing a given game room.
		 *
		 * @param arg A pointer to a Room object.
		 */
		static void* roomProcess(void *arg);

		/**
		 * Opens a new game room with the given parameters.
		 *
		 * @param room The room to open.
		 */
		void openRoom(Room *room);

		/**
		 * Closes the given game room.
		 *
		 * @param room The room to close.
		 */
		void closeRoom(Room *room);

		/**
		 * Adds a player to a game room.
		 *
		 * @param gid The room's id number.
		 * @param username The user's username.
		 * @param socket The user's connection socket.
		 * @param error This gets set to a description of the error that occurred, if any.
		 * @return true if the user was able to join, false otherwise.
		 */
		bool addPlayerToRoom(int gid, const std::string &username, int socket, std::string &error);

	private:
		/// Storage object for threads and their associated data.
		class ThreadData {
			public:
				/// Default constructor
				ThreadData() { };

				/**
				 * Creates a new thread data object for the given room.
				 *
				 * @param handle A handle to the thread.
				 * @param room The room to keep track of.
				 */
				ThreadData(Room *room);

				/**
				 * Frees data associated with this thread.
				 */
				~ThreadData();

				/// Thread handle.
				pthread_t thread;

				/// Conditional variable used to wait for the room owner to join.
				pthread_cond_t ownerJoinCV;

				/// Associated mutex with the ownerJoinCV conditional variable.
				pthread_mutex_t ownerJoinMutex;

				/// Room managed by this thread.
				Room *room;

				/// Temporary holding pointer for the room owner.
				Human *owner;
		};

	private:
		std::map<int, ThreadData*> m_Rooms;
};

#endif
