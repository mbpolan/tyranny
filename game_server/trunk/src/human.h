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
// human.h: definition of the Human class.

#ifndef HUMAN_H
#define HUMAN_H

#include <iostream>

#include "player.h"

/**
 * A model for a human player.
 * Human players have distinct attributes that this class keeps track of, namely
 * the socket that the user is connected on, and the user's username. Moreover, special
 * methods for dealing with the client are included, including the ability to chat, wait
 * on moves, and more.
 */
class Human: public Player {
	public:
		/**
		 * Creates a human player model with the given username and socket.
		 *
		 * @param username The human player's username.
		 * @param socket The socket the player connected with.
		 */
		Human(const std::string &username, int socket);

		/**
		 * Returns the player's socket file descriptor.
		 *
		 * @return A file descriptor.
		 */
		int getSocket() const { return m_Socket; }

		/**
		 * Flags this player as accepted in a room.
		 *
		 * @param b true if yes, false if no.
		 */
		void setAccepted(bool b) { m_Accepted=b; }

		/**
		 * Checks whether or not the player has been accepted by the room owner.
		 *
		 * @return true if yes, false otherwise.
		 */
		bool isAccepted() const { return m_Accepted; }

		/**
		 * Tells the client to wait for the owner to begin the game.
		 */
		void sendStartControl();

	private:
		/// The communications socket.
		int m_Socket;

		/// Flags whether the player has been accepted to join by room owner.
		bool m_Accepted;
};

#endif
