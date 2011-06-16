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

#include <sys/time.h>

#include "human.h"
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

	pthread_mutex_lock(&data->ownerJoinMutex);
	pthread_cond_timedwait(&data->ownerJoinCond, &data->ownerJoinMutex, &ts);
	pthread_mutex_unlock(&data->ownerJoinMutex);

	// see if anyone joined
	std::vector<Player*> players=data->room->getPlayers();
	if (players.empty() || players[0]->getUsername()!=data->room->getOwner()) {
		std::cout << "Owner did not join and thread timed out.\n";

		if (!players.empty())
			std::cout << "Joinee: " << players[0]->getUsername() << ", owner: " << data->room->getOwner() << std::endl;
		pthread_exit(0);
	}

	std::cout << "Owner " << players[0]->getUsername() << " joined the room.\n";

	// temporary placeholder to keep the room alive for a bit
	sleep(5);

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
	if (room->getPlayers().empty() && room->getOwner()!=username) {
		unlock();
		error="Owner must join this room first.";
		return false;
	}

	// make sure the room is not full already
	if (room->getPlayers().size()==room->getRules().getMaxHumans()) {
		unlock();
		error="This room is already full.";
		return false;
	}

	// otherwise this player is free to join
	Human *human=new Human(username, socket);
	room->addPlayer(static_cast<Player*>(human));

	// see if the owner joined for the first time, and if so, wake the room thread
	if (room->getPlayers().size()==1 && room->getOwner()==username) {
		std::cout << "Awaking on owner join condition... ";
		pthread_mutex_lock(&m_Rooms[gid]->ownerJoinMutex);
		pthread_cond_signal(&m_Rooms[gid]->ownerJoinCond);
		pthread_mutex_unlock(&m_Rooms[gid]->ownerJoinMutex);
		std::cout << "done!\n";
	}

	unlock();

	return true;
}

RoomEngine::ThreadData::ThreadData(Room *room) {
	pthread_mutex_init(&ownerJoinMutex, NULL);
	pthread_cond_init(&ownerJoinCond, NULL);

	this->room=room;
}

RoomEngine::ThreadData::~ThreadData() {
	pthread_mutex_destroy(&ownerJoinMutex);
	pthread_cond_destroy(&ownerJoinCond);
}
