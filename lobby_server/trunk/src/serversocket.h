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
// serversocket.h: declarations of the ServerSocket class

#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include <iostream>

/**
 * An simple socket class for servers.
 * This class provides an API that wraps the regular C socket functions. It
 * aims to conform to proper C++ constructs by allowing the programmer to
 * avoid having to deal with potential errors caused by direct use of the
 * C function calls. In case of errors, exceptions are thrown.
 */
class ServerSocket {
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
				std::string message() const { return m_Message; }
			private:
				/// The message for this exception.
				std::string m_Message;
		};
		
		/**
		 * Data about a client connection.
		 */
		class Client {
			public:
				/** 
				 * Default constructor.
				 * @param ip The IP address of the client.
				 * @param port The port of the client connection.
				 * @param socket The socket associated with this client.
				 */
				Client(const std::string &ip, int port, int socket): m_IP(ip), m_Port(port), m_Socket(socket) { }
				
				/**
				 * Returns the IP address of this client.
				 * @return IP address of the client.
				 */
				std::string getIP() const { return m_IP; }
				
				/**
				 * Returns the port of the client connection.
				 * @return The port of the client's connection.
				*/
				int getPort() const { return m_Port; }
				
				/**
				 * Returns the socket for this client connection.
				 * @return The client's socket.
				*/
				int getSocket() const { return m_Socket; }
			
			private:
				/// The client' IP address.
				std::string m_IP;
				
				/// The client's port number.
				int m_Port;
				
				/// The client's socket.
				int m_Socket;
		};
	
	public:
		/**
		 * Default constructor for the ServerSocket class.
		 * @param ip The IP address to bind this socket to.
		 * @param port The port to bind this socket to.
		*/
		ServerSocket(const std::string &ip, int port);
		
		/**
		 * Attempts to bind this socket to the given IP address and port.
		 * @throw A ServerSocket::Exception if an error occurs.
		 */
		void bind() throw(ServerSocket::Exception);
		
		/**
		 * Attempts to start listening for connections.
		 * @throw A ServerSocket::Exception if an error occurs.
		 */
		void listen() throw(ServerSocket::Exception);
		
		/**
		 * Waits for connections and fires a callback if one is pending.
		 * @param cb The function to call if a client connection is waiting.
		 */
		void acceptLoop(void* (*cb)(void*));
	
	private:
		/// The IP address to bind this socket to.
		std::string m_IP;
		
		/// The port to bind this socket to.
		int m_Port;
		
		/// The actual UNIX socket.
		int m_Socket;
};

#endif

