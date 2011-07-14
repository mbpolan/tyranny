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
// clientsocket.h: definition of the ClientSocket class.

#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <iostream>

class ClientSocket {
	public:
		/**
		 * A general exception for sockets.
		 */
		class Exception {
			public:
				/// Default constructor for socket exceptions.
				Exception(const std::string &msg): m_Message(msg) { };

				/**
				 * Returns the reason for this exception.
				 * @return Message string
				 */
				std::string getMessage() const { return m_Message; }
		private:
			/// The message for this exception.
			std::string m_Message;
	};

	public:
		/**
		 * Creates a new client socket.
		 */
		ClientSocket();

		/**
		 * Attempts to establish a connection to the given host.
		 *
		 * @param host The host to connect to.
		 * @param port The port number of the host.
		 * @throw Exception If an error occurred during connection.
		 */
		void connect(const std::string &host, int port) throw(ClientSocket::Exception);

		/**
		 * Disconnects from the current host.
		 *
		 * @throw Exception If an error occurs during disconnect.
		 */
		void disconnect() throw(ClientSocket::Exception);

		/**
		 * Returns the socket's file descriptor.
		 *
		 * @return The socket fd.
		 */
		int getFD() const { return m_Socket; }

	private:
		/// The socket we are working on.
		int m_Socket;
};

#endif
