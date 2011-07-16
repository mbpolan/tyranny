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
// roomengine.cpp: implementation of the RoomEngine class.

#include <sys/socket.h>
#include <sys/time.h>

#include "configfile.h"
#include "clientsocket.h"
#include "human.h"
#include "game.h"
#include "packet.h"
#include "protspec.h"
#include "roomengine.h"

RoomEngine *g_RoomEngine=NULL;

RoomEngine::RoomEngine() {
	g_RoomEngine=this;
}

RoomEngine* RoomEngine::instance() {
	return g_RoomEngine;
}

void* RoomEngine::roomProcess(void *arg) {
	ThreadData *data=(ThreadData*) arg;
	Room *room=data->room;

	std::cout << "Waiting for owner on room #" << room->getGid() << std::endl;

	// we now wait for the room owner to join
	room->waitOnJoin();

	// see if anyone joined
	std::vector<Player*> players=room->getPlayers();
	if (players.empty() || players[0]->getUsername()!=room->getOwner()) {
		std::cout << "Owner did not join and thread timed out.\n";

		if (!players.empty())
			std::cout << "Joinee: " << players[0]->getUsername() << ", owner: " << data->room->getOwner() << std::endl;

		room->unlock(Room::JoinMutex);
		pthread_exit(0);
	}

	std::cout << "Owner " << players[0]->getUsername() << " joined the room.\n";

	// tell the owner's client to show the begin/wait dialog
	Human *owner=dynamic_cast<Human*>(players[0]);
	owner->getProtocol()->sendPlayerJoined(owner->getUsername(), 0);
	owner->getProtocol()->sendStartControl();

	// set a time out of 5 seconds on the owner socket
	int osock=owner->getProtocol()->getSocket();

	struct timespec ts;
	ts.tv_sec=5;
	ts.tv_nsec=0;
	if (setsockopt(osock, SOL_SOCKET, SO_RCVTIMEO, (char*) &ts, sizeof(ts))<0) {
		std::cout << "Failed to set timeout on owner socket.\n";
	}

	room->unlock(Room::JoinMutex);

	// wait for the owner to reply
	Packet r;
	bool terminate=false;
	while(1) {
		Packet::Result result=r.read(osock);

		// if the owner disconnected, destroy the room
		if (result==Packet::Disconnected || result==Packet::DataCorrupt) {
			std::cout << "Owner connection terminated\n";
			terminate=true;
			break;
		}

		// otherwise if the read timed out, check for new players
		else if (result==Packet::TimedOut) {
			room->lock(Room::JoinMutex);

			std::cout << "Checking for new players...\n";

			// go over the list of players and check who is not accepted
			for (int i=1; i<4; i++) {
				Human *hp=dynamic_cast<Human*>(room->getPlayers()[i]);
				if (!hp)
					continue;

				// first see if this player is still connected
				Packet test;
				Packet::Result alive=test.timedRead(hp->getProtocol()->getSocket(), 1, 0);
				if (alive==Packet::Disconnected) {
					// remove him from the players
					std::cout << hp->getUsername() << " disconnected.\n";
					int index=room->removePlayer(hp);

					// let all the other players know
					for (int j=0; j<4; j++) {
						Human *op=dynamic_cast<Human*>(room->getPlayers()[j]);
						if (op)
							op->getProtocol()->sendPlayerQuit(index);
					}

					delete hp;
				}

				// accept this player if the owner agrees
				else if (!hp->isAccepted()) {
					std::cout << hp->getUsername() << " joined the room!\n";
					hp->setAccepted(true);

					int index=room->getPlayerIndex(hp->getUsername());

					// let all the other players know, and update the joined player
					for (int j=0; j<4; j++) {
						Player *pl=room->getPlayers()[j];
						if (pl) {
							hp->getProtocol()->sendPlayerJoined(pl->getUsername(), j);

							// if this player is not the joined player, tell them that someone joined
							Human *op=dynamic_cast<Human*>(pl);
							if (op && op->getUsername()!=hp->getUsername())
								op->getProtocol()->sendPlayerJoined(hp->getUsername(), index);
						}
					}
				}
			}

			room->unlock(Room::JoinMutex);
		}

		// otherwise the owner sent a packet
		else {
			uint8_t header=r.byte();

			// see if this is the reply we are waiting for
			if (header==GMRM_BEGIN_GAME) {
				std::cout << "Owner flagged game to begin\n";
				break;
			}
		}
	}

	// see if we should begin the game
	if (!terminate) {
		// remove any timeouts we set on the owner socket
		ts.tv_sec=0;
		ts.tv_nsec=0;
		if (setsockopt(osock, SOL_SOCKET, SO_RCVTIMEO, (char*) &ts, sizeof(ts))<0) {
			std::cout << "Failed to clear timeout on owner socket.\n";
		}

		std::cout << "Beginning new round\n";

		// create a new game handler for this room and start it up
		Game game(data->room);
		game.begin();

		sleep(10);
	}

	// tell the lobby server that this room is closing
	try {
		ClientSocket cl;
		cl.connect(ConfigFile::instance()->getLobbyServerIP(), ConfigFile::instance()->getLobbyServerPort());

		Packet lp;
		lp.addByte(CONN_GAME);
		lp.addByte(IS_KILLROOM);
		lp.addUint32(data->room->getGid());
		lp.write(cl.getFD());

		cl.disconnect();
	}

	catch (const ClientSocket::Exception &ex) {
		std::cout << "ClientSocket Error: " << ex.getMessage() << std::endl;
	}

	// now disconnect all players
	for (int i=0; i<4; i++) {
		Human *hp=dynamic_cast<Human*>(room->getPlayers()[i]);
		if (hp) {
			close(hp->getProtocol()->getSocket());
			delete hp;
		}
	}

	std::cout << "Ending room #" << room->getGid() << std::endl;

	g_RoomEngine->closeRoom(room);
	delete room;

	pthread_exit(0);
}

void RoomEngine::openRoom(Room *room) {
	lock();

	// create a new thread data object
	ThreadData *data=new ThreadData(room);

	// add the room to the map
	m_Rooms[room->getGid()]=data;

	// start a new thread for this room
	pthread_create(&data->thread, NULL, &RoomEngine::roomProcess, (void*) data);

	unlock();
}

void RoomEngine::closeRoom(Room *room) {
	lock();

	m_Rooms.erase(room->getGid());

	ThreadData *data=m_Rooms[room->getGid()];
	delete data;

	unlock();
}

bool RoomEngine::addPlayerToRoom(int gid, const std::string &username, int socket, std::string &error) {
	lock();

	// verify the room exists
	if (m_Rooms.find(gid)==m_Rooms.end()) {
		unlock();
		error="No such room exists.";
		return false;
	}

	Room *room=m_Rooms[gid]->room;

	// if the room is empty, the owner must join first
	if (room->getNumPlayers()==0 && room->getOwner()!=username) {
		unlock();
		error="Owner must join this room first.";
		return false;
	}

	// make sure the room is not full already
	if (room->getNumPlayers()==room->getRules().getMaxHumans()) {
		unlock();
		error="This room is already full.";
		return false;
	}

	m_Rooms[gid]->room->lock(Room::JoinMutex);

	std::cout << "Preparing new human player object...\n";

	// otherwise this player is free to join
	Human *human=new Human(username, socket);
	human->setAccepted(false);
	room->addPlayer(dynamic_cast<Player*>(human));

	// see if the owner joined for the first time, and if so, wake the room thread
	if (room->getOwner()==username) {
		std::cout << "Awaking on owner join condition. ";
		m_Rooms[gid]->room->awakeOnJoin();
	}

	m_Rooms[gid]->room->unlock(Room::JoinMutex);

	unlock();

	return true;
}

RoomEngine::ThreadData::ThreadData(Room *room) {
	this->room=room;
}

RoomEngine::ThreadData::~ThreadData() {
}
