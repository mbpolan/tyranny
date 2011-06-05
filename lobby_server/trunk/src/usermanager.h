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

#include "user.h"

class UserManager {
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
		 * Sends a chat message to all clients from another user.
		 *
		 * @param user The user who sent the message.
		 * @param message The contents of the message.
		 */
		void broadcastChatMessage(const std::string &user, const std::string &message);

	private:
		/// Map of users, hashed according to their usernames.
		std::map<std::string, User*> m_UserMap;

		/// Synchronization variables.
		pthread_mutex_t m_Mutex;
};

#endif
