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
// protocol.h: definition of the Protocol class.

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "packet.h"

class User;

class Protocol {
	public:
		/**
		 * Default constructor.
		 * @param socket Socket to associate with this protocol.
		 */
		Protocol(int socket);

		/**
		 * Sets the user associated with this protocol.
		 *
		 * @param user The user to associate.
		 */
		void setUser(User *user) { m_User=user; }

		/**
		 * Begins the communication loop with the client.
		 */
		void communicationLoop();

		/**
		 * Sends this user's client a packet containing the details of a new user who logged in.
		 *
		 * @param user The user who just logged in.
		 */
		void sendUserLoggedIn(User *other);

		/**
		 * Sends this user's client a packet containing the details of a user who logged out.
		 *
		 * @param user The user who just logged out.
		 */
		void sendUserLoggedOut(User *other);

		/**
		 * Sends this user a chat message from another user.
		 *
		 * @param user The user who sent the message.
		 * @param message The contents of the message.
		 */
		void sendChatMessage(const std::string &user, const std::string &message);

	private:
		/**
		 * Parses and evaluates a given packet.
		 *
		 * @param p The packet to parse.
		 */
		void parsePacket(Packet &p);

		/**
		 * Handles a user sending a chat message.
		 *
		 * @param p The packet to parse.
		 */
		void handleUserChatMessage(Packet &p);

		/**
		 * Handles gathering and sending a user his/her statistics.
		 *
		 * @param p The packet to parse.
		 */
		void handleStatistics(Packet &p);

		/**
		 * Handles updating a user's profile data.
		 *
		 * @param p The packet to parse.
		 */
		void handleUserProfileRequest(Packet &p);

		/**
		 * Handles updating a user's profile.
		 *
		 * @param p The packet to parse.
		 */
		void handleUserProfileUpdate(Packet &p);

		/// The user associated with this protocol.
		User *m_User;

		/// Socket for reading/writing data between the server and client.
		int m_Socket;
};

#endif
