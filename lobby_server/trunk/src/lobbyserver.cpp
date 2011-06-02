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
#include <pthread.h>

#include "configfile.h"
#include "lobbyserver.h"
#include "packet.h"
#include "protspec.h"
#include "protocol.h"
#include "serversocket.h"

// globals
ConfigFile *g_ConfigFile;

void* connectionHandler(void *arg) {
	ServerSocket::Client *data=(ServerSocket::Client*) arg;
	int socket=data->getSocket();
	
	std::cout << "Accepted connection from " << data->getIP() << ":" << data->getPort() << std::endl;
	
	// send the client an authentication request
	Packet p, rp;
	p.addByte(AUTH_REQUEST);
	p.write(socket);

	// wait for a response
	rp.read(socket);

	// verify the packet
	if (rp.byte()==AUTH_DATA) {
		Packet p2;
		std::string username=rp.string();
		std::string password=rp.string();

		// dummy values for authentication testing
		if (username=="user" && password=="test") {
			p2.addByte(AUTH_SUCCESS);
			p2.write(socket);
		}

		// test authentication failures
		else {
			p2.addByte(AUTH_ERROR);
			p2.addString("Incorrect username or password.");
			p2.write(socket);
		}
	}

	// otherwise alert the client
	else {
		Packet p2;
		p2.addByte(AUTH_ERROR);
		p2.addString("Unexpected response packet.");
		p2.write(socket);
	}

	sleep(5);

	close(socket);
	
	std::cout << "Disconnected client on socket " << socket << std::endl;
	
	delete data;
	pthread_exit(0);
}

int main(int argc, char *argv[]) {
	// try to parse the configuration file
	g_ConfigFile=new ConfigFile("config.xml");

	std::cout << "Loading configuration file...\t";
	try {
		g_ConfigFile->parse();
	}

	catch (const ConfigFile::Exception &ex) {
		std::cout << "[fail]\n";
		std::cout << ex.getMessage() << std::endl;

		exit(1);
	}

	std::cout << "[done]\n";
	
	// create the server socket and activate it
	ServerSocket sock(g_ConfigFile->getIP(), g_ConfigFile->getPort());
	
	std::cout << "Creating server socket...\t";
	try {
		sock.bind();
		sock.listen();
	}
	catch (const ServerSocket::Exception &ex) {
		std::cout << "[fail]\n";
		std::cout << ex.getMessage() << std::endl;

		exit(0);
	}
	
	std::cout << "[done]\n";

	// print out some status messages
	std::cout << "Tyranny Lobby Server " << LOBBY_SERVER_VERSION << " running...\n";
	
	// start waiting for connections
	while(1) {
		ServerSocket::Client *cl=sock.accept();

		// if a client connected, start a new thread to handle the connection
		if (cl) {
			pthread_t th;
			pthread_create(&th, NULL, &connectionHandler, cl);
		}
	}
	
	return 0;
}

