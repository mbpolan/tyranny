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
// serverpool.h: definition of the ServerPool class.

#ifndef SERVERPOOL_H
#define SERVERPOOL_H

#include <iostream>
#include <map>

class ServerPool {
	public:
		/// Default constructor.
		ServerPool();

		/// Frees memory associated with this object.
		~ServerPool();

		/**
		 * Returns an instance of this class.
		 *
		 * @return A pointer to a ServerPool object.
		 */
		static ServerPool* instance();

		/**
		 * Adds a game server to the pool.
		 *
		 * @param id The identifier for the server.
		 * @param host The hostname or IP address of the server.
		 * @param port The port number of the server.
		 */
		void addGameServer(const std::string &id, const std::string &host, int port);

		/**
		 * Gets the connection information for a given server.
		 *
		 * @param id The identifier of the game server.
		 * @param host Sets this value to the host or IP address of the server.
		 * @param port Sets this value to the port number of the server.
		 */
		void getGameServer(const std::string &id, std::string &host, int &port);

		/**
		 * Finds the most suitable server for a new client connection.
		 *
		 * @param host Sets this value to the host or IP address of the server.
		 * @param port Sets this value to the port number of the server.
		 */
		void selectGameServer(std::string &host, int &port);

	private:
		class GameServer {
			public:
				/**
				 * Creates a game server data object.
				 *
				 * @param id The identifier for the server.
				 * @param host The hostname or IP address of the server.
				 * @param port The port number of the server.
				 */
				GameServer(const std::string &id, const std::string &host, int port) {
					m_ID=id;
					m_Host=host;
					m_Port=port;
				}

				/**
				 * Returns the identifier string for this game server.
				 *
				 * @return The id string.
				 */
				std::string getID() const { return m_ID; }

				/**
				 * Returns the hostname or IP address of this game server.
				 *
				 * @return The server's hostname/IP address.
				 */
				std::string getHost() const { return m_Host; }

				/**
				 * Returns the port number of this game server.
				 *
				 * @return The server's port.
				 */
				int getPort() const { return m_Port; }

			private:
				/// The server's identifier.
				std::string m_ID;

				/// The hostname/IP address.
				std::string m_Host;

				/// The port number.
				int m_Port;
		};

	private:
		std::map<std::string, GameServer*> m_Servers;
};

#endif
