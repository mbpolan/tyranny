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
// lobbyserver.cpp: entry point for the lobby server

#include <iostream>
#include <cstdlib>

#include "lobbyserver.h"
#include "serversocket.h"

void* connectionHandler(void *arg) {
	ServerSocket::Client *data=(ServerSocket::Client*) arg;
	
	std::cout << "Accepted connection from " << data->getIP() << ":" << data->getPort() << std::endl;
	
	// temporary placeholder for testing connections
	sleep(5);
	
	std::cout << "Disconnected client on socket " << data->getSocket() << std::endl;
	
	delete data;
	pthread_exit(0);
}

int main(int argc, char *argv[]) {
	// require ip address and port for now
	if (argc!=3) {
		std::cout << "Usage: tyranny-lobby-server <ip> <port>\n";
		return 0;
	}
	
	// create the server socket and activate it
	ServerSocket sock(argv[1], atoi(argv[2]));
	
	try {
		sock.bind();
		sock.listen();
	}
	catch (const ServerSocket::Exception &ex) {
		return 0;
	}
	
	// print out some status messages
	std::cout << "Tyranny Lobby Server " << LOBBY_SERVER_VERSION << " running...\n";
	
	// start waiting for connections
	sock.acceptLoop(&connectionHandler);
	
	return 0;
}

