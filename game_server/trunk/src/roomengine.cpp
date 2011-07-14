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

	std::cout << "Waiting for owner on room #" << data->room->getGid() << std::endl;

	// initially set a 5 minute time out and wait for the owner to join
	struct timeval tv;
	gettimeofday(&tv, NULL);

	struct timespec ts;
	ts.tv_sec=tv.tv_sec+(60*5);
	ts.tv_nsec=tv.tv_usec*1000;

	pthread_mutex_lock(&data->joinMutex);
	pthread_cond_timedwait(&data->ownerJoinCond, &data->joinMutex, &ts);
	pthread_mutex_unlock(&data->joinMutex);

	// see if anyone joined
	std::vector<Player*> players=data->room->getPlayers();
	if (players.empty() || players[0]->getUsername()!=data->room->getOwner()) {
		std::cout << "Owner did not join and thread timed out.\n";

		if (!players.empty())
			std::cout << "Joinee: " << players[0]->getUsername() << ", owner: " << data->room->getOwner() << std::endl;
		pthread_exit(0);
	}

	std::cout << "Owner " << players[0]->getUsername() << " joined the room.\n";

	// tell the owner's client to show the begin/wait dialog
	Human *owner=static_cast<Human*>(players[0]);
	owner->sendStartControl();

	// set a time out of 5 seconds on the owner socket
	int osock=owner->getSocket();
	ts.tv_sec=5;
	ts.tv_nsec=0;
	if (setsockopt(osock, SOL_SOCKET, SO_RCVTIMEO, (char*) &ts, sizeof(ts))<0) {
		std::cout << "Failed to set timeout on owner socket.\n";
	}

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
			pthread_mutex_lock(&data->joinMutex);

			std::cout << "Checking for new players...\n";

			// go over the list of players and check who is not accepted
			for (int i=1; i<4; i++) {
				Human *hp=static_cast<Human*>(data->room->getPlayers()[i]);
				if (!hp)
					continue;

				// first see if this player is still connected
				Packet test;
				Packet::Result alive=test.timedRead(hp->getSocket(), 1, 0);
				if (alive==Packet::Disconnected) {
					// remove him from the players
					std::cout << hp->getUsername() << " disconnected.\n";
					data->room->removePlayer(hp);

					delete hp;
				}

				// accept this player if the owner agrees
				else if (!hp->isAccepted()) {
					std::cout << hp->getUsername() << " joined the room!\n";
					hp->setAccepted(true);
				}
			}

			pthread_mutex_unlock(&data->joinMutex);
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
	}

	// temporary placeholder to keep the room alive for a bit
	sleep(5);

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

	std::cout << "Ending room #" << data->room->getGid() << std::endl;

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
	ThreadData *data=m_Rooms[room->getGid()];
	delete data;

	m_Rooms.erase(room->getGid());
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

	pthread_mutex_lock(&m_Rooms[gid]->joinMutex);

	std::cout << "Preparing new human player object...\n";

	// otherwise this player is free to join
	Human *human=new Human(username, socket);
	human->setAccepted(false);
	room->addPlayer(static_cast<Player*>(human));

	// see if the owner joined for the first time, and if so, wake the room thread
	if (room->getOwner()==username) {
		std::cout << "Awaking on owner join condition. ";
		pthread_cond_signal(&m_Rooms[gid]->ownerJoinCond);
	}

	pthread_mutex_unlock(&m_Rooms[gid]->joinMutex);

	unlock();

	return true;
}

RoomEngine::ThreadData::ThreadData(Room *room) {
	pthread_mutex_init(&joinMutex, NULL);
	pthread_cond_init(&ownerJoinCond, NULL);

	this->room=room;
}

RoomEngine::ThreadData::~ThreadData() {
	pthread_mutex_destroy(&joinMutex);
	pthread_cond_destroy(&ownerJoinCond);
}
