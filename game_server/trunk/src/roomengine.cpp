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
#include "thread.h"

RoomEngine *g_RoomEngine=NULL;

RoomEngine::RoomEngine() {
	g_RoomEngine=this;
}

RoomEngine* RoomEngine::instance() {
	return g_RoomEngine;
}

void* RoomEngine::roomProcess(void *arg) {
	ThreadData *data=(ThreadData*) arg;

	std::cout << "New thread started...\n";

	// now wait on the owner for up to 5 minutes
	Thread::lock(&data->ownerJoinMutex);
	data->room->setPhase(Room::AwaitOwner);

	Thread::sleep(&data->ownerJoinCV, &data->ownerJoinMutex, 7);

	// determine if the owner joined or not
	bool begin=false;
	if (data->owner) {
		data->room->addOwner(data->owner);
		begin=true;
	}

	Thread::unlock(&data->ownerJoinMutex);

	// start the game room now
	if (begin)
		data->room->begin();

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

	std::cout << "Terminating game room " << data->room->getGid() << std::endl;

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

	ThreadData *data=m_Rooms[gid];
	Thread::lock(&data->ownerJoinMutex);

	Room *room=data->room;

	// create a new human object
	Human *hp=new Human(username, socket);

	// if the room is awaiting its owner, join the player if he is in fact that owner
	if (room->getPhase()==Room::AwaitOwner) {
		if (username==room->getOwner()) {
			// also wake the room process thread
			data->owner=hp;
			Thread::signal(&data->ownerJoinCV);
		}

		else {
			error="This is not the room owner";
			Thread::unlock(&data->ownerJoinMutex);
			unlock();

			return false;
		}
	}

	// other phases when the player could join
	else
		room->queuePlayer(hp);

	Thread::unlock(&data->ownerJoinMutex);

	unlock();
	return true;
}

RoomEngine::ThreadData::ThreadData(Room *room) {
	this->room=room;
	Thread::createCondVar(&ownerJoinCV);
	Thread::createMutex(&ownerJoinMutex);
	owner=NULL;
}

RoomEngine::ThreadData::~ThreadData() {
	Thread::destroyCondVar(&ownerJoinCV);
	Thread::destroyMutex(&ownerJoinMutex);
}
