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
// room.cpp: implementation of the Room class.

#include <sys/time.h>

#include "human.h"
#include "room.h"
#include "utilities.h"

Room::Room(int gid, const std::string &owner) {
	m_Gid=gid;
	m_Owner=owner;
	m_Rules=Rules(0, 0, 0, false, Rules::RandomToPlayers);
	m_Players=std::vector<Player*>(4);
	m_TurnOrder=std::vector<int>(4);

	for (int i=0; i<4; i++) {
		m_Players[i]=NULL;
		m_TurnOrder[i]=-1;
	}

	// initialize sync variables
	pthread_mutex_init(&m_JoinMutex, NULL);
	pthread_cond_init(&m_OwnerJoinCond, NULL);
}

void Room::randomizeTurnOrder() {
	Lockable::lock();

	// TODO: make this actually random
	m_TurnOrder[0]=0;
	m_TurnOrder[1]=1;
	m_TurnOrder[2]=2;
	m_TurnOrder[3]=3;

	// inform all human players
	for (int i=0; i<4; i++) {
		Human *hp=static_cast<Human*>(m_Players[i]);
		if (hp)
			hp->getProtocol()->sendTurnOrder(m_TurnOrder);
	}

	Lockable::unlock();
}

void Room::waitOnJoin() {
	// initially set a 5 minute time out and wait for the owner to join
	struct timeval tv;
	gettimeofday(&tv, NULL);

	struct timespec ts;
	ts.tv_sec=tv.tv_sec+(60*5);
	ts.tv_nsec=tv.tv_usec*1000;

	pthread_mutex_lock(&m_JoinMutex);
	pthread_cond_timedwait(&m_OwnerJoinCond, &m_JoinMutex, &ts);
}

void Room::awakeOnJoin() {
	pthread_cond_signal(&m_OwnerJoinCond);
}

void Room::lock(const Room::Lock &lock) {
	switch(lock) {
		case Room::JoinMutex: pthread_mutex_lock(&m_JoinMutex); break;
	}
}

void Room::unlock(const Room::Lock &lock) {
	switch(lock) {
		case Room::JoinMutex: pthread_mutex_unlock(&m_JoinMutex); break;
	}
}

void Room::setRules(const Room::Rules &rules) {
	Lockable::lock();

	m_Rules=rules;

	Lockable::unlock();
}

Room::Rules Room::getRules() {
	Room::Rules rules;

	Lockable::lock();

	rules=m_Rules;

	Lockable::unlock();

	return rules;
}

int Room::getPlayerIndex(const std::string &username) {
	Lockable::lock();

	for (int i=0; i<4; i++) {
		if (m_Players[i] && m_Players[i]->getUsername()==username) {
			Lockable::unlock();
			return i;
		}
	}

	Lockable::unlock();
	return -1;
}

void Room::addPlayer(Player *player) {
	Lockable::lock();

	// find an empty slot
	for (int i=0; i<4; i++) {
		if (!m_Players[i]) {
			m_Players[i]=player;
			break;
		}
	}

	Lockable::unlock();
}

int Room::removePlayer(const Player *player) {
	Lockable::lock();

	int index=-1;
	for (int i=0; i<4; i++) {
		if (m_Players[i]==player) {
			index=i;
			m_Players[i]=NULL;
		}
	}

	Lockable::unlock();
	return index;
}

std::vector<Player*> Room::getPlayers() {
	std::vector<Player*> list;

	Lockable::lock();
	list=m_Players;
	Lockable::unlock();

	return list;
}

int Room::getNumPlayers() {
	Lockable::lock();

	int count=0;

	for (int i=0; i<4; i++) {
		if (m_Players[i])
			count++;
	}

	Lockable::unlock();

	return count;
}
