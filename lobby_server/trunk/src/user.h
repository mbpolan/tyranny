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
// user.h: definition of the User class.

#ifndef USER_H
#define USER_H

#include <iostream>
#include <vector>

#include "protocol.h"

/**
 * A model for a connected user.
 * Once a client establishes a successful connection to the server, his
 * or her data is then loaded from the database and placed in a User object
 * for use in the lobby server operations. The required parameters are the
 * username (which must be unique) and password.
 */
class User {
	public:
		/**
		 * Creates a model for a generic user.
		 * Each user must have a unique username.
		 *
		 * @param username This user's unique username.
		 * @param password This user's password.
		 */
		User(const std::string &username, const std::string &password);

		/**
		 * Sets the email address for this user, if he/she has one.
		 *
		 * @param email The user's email address, or a blank string if none exists.
		 */
		void setEmail(const std::string &email) { m_Email=email; }

		/**
		 * Returns the user's assigned email address.
		 *
		 * @return The user's email address, or an empty string if none exists.
		 */
		std::string getEmail() const { return m_Email; }

		/**
		 * Controls whether or not the user is muted from the general chat.
		 *
		 * @param muted True if yes, false otherwise.
		 */
		void setIsMuted(bool muted) { m_Muted=muted; }

		/**
		 * Checks whether or not the user is muted.
		 *
		 * @return True if so, false otherwise.
		 */
		bool isMuted() const { return m_Muted; }

		/**
		 * Returns the username for this user.
		 *
		 * @return The username identifying this user.
		 */
		std::string getUsername() const { return m_Username; }

		/**
		 * Returns the password for this user.
		 *
		 * @return The user's password.
		 */
		std::string getPassword() const { return m_Password; }

		/**
		 * Sets this user's assigned protocol object.
		 *
		 * @param protocol The protocol assigned to this user.
		 */
		void setProtocol(Protocol *protocol) { m_Protocol=protocol; }

		/**
		 * Returns this user's communications protocol.
		 *
		 * @return The user's protocol.
		 */
		Protocol* getProtocol() const { return m_Protocol; }

		/**
		 * Sets this user's friend list.
		 *
		 * @param list A list of usernames.
		 */
		void setFriendList(const std::vector<std::string> &list) { m_Friends=list; }

		/**
		 * Returns this user's friend list.
		 *
		 * @return A list of usernames.
		 */
		std::vector<std::string> getFriendList() const { return m_Friends; }

		/**
		 * Sets this user's blocked user list.
		 *
		 * @param list A list of usernames.
		 */
		void setBlockedList(const std::vector<std::string> &list) { m_Blocked=list; }

		/**
		 * Returns this user's blocked user list.
		 *
		 * @return A list of usernames.
		 */
		std::vector<std::string> getBlockedList() const { return m_Blocked; }

	private:
		/// The user's username.
		std::string m_Username;

		/// The user's password.
		std::string m_Password;

		/// The user's email address.
		std::string m_Email;

		/// Whether or not the user is muted.
		bool m_Muted;

		/// The user's protocol object.
		Protocol *m_Protocol;

		/// A cache of this user's friend list.
		std::vector<std::string> m_Friends;

		/// A cache of this user's blocked user list.
		std::vector<std::string> m_Blocked;
};

#endif
