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
 // gameserver.cpp: main entry point into the program.
 
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sys/socket.h>
 
#include "configfile.h"
#include "gameserver.h"
#include "protspec.h"
#include "room.h"
#include "roomengine.h"
#include "serversocket.h"
 
// globals
ConfigFile *g_ConfigFile=NULL;
RoomEngine *g_Engine=NULL;

void* connectionHandler(void*);
void handleClientConnection(Packet &p, ServerSocket::Client*);
void handleLobbyServerConnection(Packet &p, ServerSocket::Client*);

void* connectionHandler(void *arg) {
	ServerSocket::Client *data=(ServerSocket::Client*) arg;
	int socket=data->getSocket();

	// read an initial packet
	Packet p;
	if (p.read(socket)!=Packet::NoError) {
		close(socket);
		pthread_exit(0);
	}

	// determine what type of connection this is
	uint8_t header=p.byte();

	// a client is attempting to connect
	if (header==CONN_CLIENT) {
		std::cout << "Accepted client connection on " << data->getIP() << ":" << data->getPort() << std::endl;
		handleClientConnection(p, data);
	}

	// the parent lobby server is attempting to connect
	else if (header==CONN_LOBBY)
		handleLobbyServerConnection(p, data);

	else {
		std::cout << "Rejecting connection from " << data->getIP() << " (unknown source)\n";
		close(socket);
	}

	delete data;
	pthread_exit(0);
}

void handleLobbyServerConnection(Packet &p, ServerSocket::Client *data) {
	int socket=data->getSocket();

	// verify that this is an authentic connection
	if (data->getIP()!=g_ConfigFile->getLobbyServerIP()) {
		std::cout << "Warning: rejecting unauthorized lobby server connection from: " << data->getIP() << std::endl;
		close(socket);

		return;
	}

	std::cout << "Accepted lobby server connection on " << data->getIP() << ":" << data->getPort() << std::endl;

	// determine the lobby server's request
	uint8_t request=p.byte();

	// open a new room on this server
	if (request==IS_OPENROOM) {
		// gather data from the packet
		int gid=p.uint32();
		std::string owner=p.string();
		char onlyFriends=p.byte();
		int maxTurns=p.uint32();
		int maxHumans=p.uint16();
		int freeParkReward=p.uint32();
		char incomeTaxChoice=p.byte();
		char propMethod=p.byte();

		// translate packet bytes to enums
		Room::Rules::RedistMethod rmethod;
		if (propMethod==PROP_RANDOM)
			rmethod=Room::Rules::RandomToPlayers;
		else
			rmethod=Room::Rules::ReturnToBank;

		// allocate a new room
		Room *room=new Room(gid, owner);
		Room::Rules rules(maxTurns, maxHumans, freeParkReward, (incomeTaxChoice==0x01), rmethod);

		room->setRules(rules);
		RoomEngine::instance()->openRoom(room);

		std::cout << "Opened a room with gid " << gid << std::endl;
	}

	std::cout << "Disconnected lobby server connection on socket " << socket << std::endl;
	close(socket);
}

void handleClientConnection(Packet &p, ServerSocket::Client *data) {
	int socket=data->getSocket();

	// get the client's username and target room id
	std::string username=p.string();
	int gid=p.uint32();

	std::cout << username << " wants to join room " << gid << std::endl;

	std::string error;
	if (!RoomEngine::instance()->addPlayerToRoom(gid, username, socket, error)) {
		std::cout << "ERROR: " << error << std::endl;
	}
}

int main(int argc, char *argv[]) {
	// parse the config file
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

	std::cout << "Creating room engine...\t";

	// create the room engine
	g_Engine=new RoomEngine();

	std::cout << "[done]\n";

	// create the server socket
	ServerSocket sock(g_ConfigFile->getIP(), g_ConfigFile->getPort());

	std::cout << "Creating server socket...\t";
	try {
		sock.bind();
		sock.listen();
	}

	catch (const ServerSocket::Exception &ex) {
		std::cout << "[fail]\n";
		std::cout << ex.getMessage() << std::endl;

		exit(1);
	}

	std::cout << "[done]\n";

	// print out some status messages
	std::cout << "Tyranny Game Server " << GAME_SERVER_VERSION << " running...\n";

	while(1) {
		ServerSocket::Client *cl=sock.accept();

		// create a new thread for this connection
		if (cl) {
			pthread_t th;
			pthread_create(&th, NULL, &connectionHandler, cl);
		}
	}

	return 0;
}
