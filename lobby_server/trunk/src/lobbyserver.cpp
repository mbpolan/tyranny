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
#include "dbmysql.h"
#include "lobbyserver.h"
#include "packet.h"
#include "protspec.h"
#include "protocol.h"
#include "serverpool.h"
#include "serversocket.h"
#include "user.h"
#include "usermanager.h"

// globals
ConfigFile *g_ConfigFile;
ServerPool *g_Pool;
UserManager *g_UserManager;

void* connectionHandler(void*);
void handleClientConnection(ServerSocket::Client*);
void handleGameServerConnection(Packet &p, ServerSocket::Client*);

void* connectionHandler(void *arg) {
	ServerSocket::Client *data=(ServerSocket::Client*) arg;
	int socket=data->getSocket();
	
	// read an initial packet
	Packet p;
	if (p.read(socket)!=Packet::NoError) {
		close(socket);
		pthread_exit(0);
	}

	uint8_t header=p.byte();

	// connection from a client
	if (header==CONN_CLIENT)
		handleClientConnection(data);

	// connection from a game server
	else if (header==CONN_GAME)
		handleGameServerConnection(p, data);

	else
		std::cout << "Unknown connection from source: " << data->getIP() << std::endl;

	pthread_exit(0);
}

void handleClientConnection(ServerSocket::Client *data) {
	int socket=data->getSocket();

	std::cout << "Accepted client connection from " << data->getIP() << std::endl;
	
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

		try {
			// create a database connection manager and try to authenticate this user
			DBMySQL db(g_ConfigFile->getDBHost(), g_ConfigFile->getDBPort(), g_ConfigFile->getDBName());
			db.connect(g_ConfigFile->getDBUser(), g_ConfigFile->getDBPassword());

			// this username-password pair must exist in the database
			if (db.authenticate(username, password)) {
				// update the client
				p2.addByte(AUTH_SUCCESS);
				p2.addString(g_ConfigFile->getName());
				p2.write(socket);

				// load this user's data from the database
				User *user=new User(username, password);
				db.loadUser(user);

				// create a new protocol object
				Protocol *p=new Protocol(socket);
				p->setUser(user);
				user->setProtocol(p);

				// add him to the pool
				g_UserManager->addUser(user);

				// send the user a list of rooms open now
				g_UserManager->sendRoomList(user->getUsername());

				// begin the communications loop
				p->communicationLoop();

				// we're done with this user
				g_UserManager->removeUser(user);
				delete user;
				delete p;
			}

			else {
				p2.addByte(AUTH_ERROR);
				p2.addString("Incorrect username or password.");
				p2.write(socket);
			}

			db.disconnect();
		}

		catch (const DBMySQL::Exception &ex) {
			std::cout << "** DBMYSQL *** : " << ex.getMessage() << std::endl;
		}
	}

	// otherwise alert the client
	else {
		Packet p2;
		p2.addByte(AUTH_ERROR);
		p2.addString("Unexpected response packet.");
		p2.write(socket);
	}

	close(socket);
	
	std::cout << "Disconnected client on socket " << socket << std::endl;
}

void handleGameServerConnection(Packet &p, ServerSocket::Client *data) {
	int socket=data->getSocket();

	// verify this connection is in fact authentic
	std::vector<ConfigFile::Server> servers=g_ConfigFile->getGameServerList();
	bool authentic=false;
	for (int i=0; i<servers.size(); i++) {
		if (servers[i].getIP()==data->getIP())
			authentic=true;
	}

	if (!authentic) {
		std::cout << "Warning: rejecting unauthorized game server connection from: " << data->getIP() << std::endl;
		close(socket);

		return;
	}

	std::cout << "Accepted game server connection from " << data->getIP() << std::endl;

	// see what the game server wants
	uint8_t action=p.byte();

	// a game room has closed
	if (action==IS_KILLROOM)
		g_UserManager->unregisterGameRoom(p.uint32());

	close(socket);
}

int main(int argc, char *argv[]) {
	// try to parse the configuration file
	g_ConfigFile=new ConfigFile("config.xml");

	std::cout << "Loading configuration file...\t";
	try {
		g_ConfigFile->parse();

		// cache database data
		DBMySQL::cache(g_ConfigFile->getDBHost(), g_ConfigFile->getDBPort(), g_ConfigFile->getDBName(),
					   g_ConfigFile->getDBUser(), g_ConfigFile->getDBPassword());
	}

	catch (const ConfigFile::Exception &ex) {
		std::cout << "[fail]\n";
		std::cout << ex.getMessage() << std::endl;

		exit(1);
	}

	std::cout << "[done]\n";
	
	// prepare the database
	std::cout << "Preparing database...\t\t";

	try {
		pDBMySQL db=DBMySQL::synthesize();
		db->prepare();
		db->disconnect();
	}

	catch (const DBMySQL::Exception &ex) {
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
	std::cout << "Creating user manager...\t";

	// create the user manager
	g_UserManager=new UserManager;

	std::cout << "[done]\n";
	std::cout << "Creating server pool...\t\t";

	// create the server pool
	g_Pool=new ServerPool;
	std::vector<ConfigFile::Server> servers=g_ConfigFile->getGameServerList();
	for (int i=0; i<servers.size(); i++)
		g_Pool->addGameServer(servers[i].getIDString(), servers[i].getIP(), servers[i].getPort());

	if (servers.empty()) {
		std::cout << "[fail]\nAt least one game server must be defined in the configuration file.\n";
		delete g_UserManager;
		delete g_ConfigFile;

		exit(1);
	}

	else
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

