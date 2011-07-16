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

#include <vector>

class Protocol {
	public:
		/**
		 * Creates a new game protocol that communicates over the given socket.
		 *
		 * @param socket The socket to perform read/write operations on.
		 */
		Protocol(int socket);

		/**
		 * Returns this protocol's socket file descriptor.
		 *
		 * @return Socket file descriptor.
		 */
		int getSocket() const { return m_Socket; }

		/**
		 * Informs the client that player with the given index number just joined the room.
		 * Each player is indexed for consistency across clients, so the server can
		 * simply send an int instead of a string when refering to players.
		 *
		 * @param username The username of the player.
		 * @param index The index of the player.
		 */
		void sendPlayerJoined(const std::string &username, int index);

		/**
		 * Informs the client that the player with the given index has disconnected.
		 *
		 * @param index The index of the disconnected player.
		 */
		void sendPlayerQuit(int index);

		/**
		 * Tells the client to wait for the owner to begin the game.
		 */
		void sendStartControl();

		/**
		 * Sends the client the layout of turns (who goes first, second, etc.)
		 * The vector should contain exactly four elements. The first element denotes
		 * the player who goes first, the second element denotes the player who goes
		 * second, and so on. Example:
		 *
		 * order[0] = 0 // player 1 goes first
		 * order[1] = 3 // player 4 goes second
		 * order[2] = 1 // player 2 goes third
		 * order[3] = 2 // player 3 goes last
		 *
		 * @param order A vector of turn orders.
		 */
		void sendTurnOrder(const std::vector<int> &order);

	private:
		/// The communications socket.
		int m_Socket;
};

#endif
