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
#include <sstream>

#include "aiplayer.h"
#include "human.h"
#include "packet.h"
#include "protspec.h"
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

void Room::assignAI() {
	Lockable::lock();

	for (int i=0; i<4; i++) {
		if (!m_Players[i]) {
			// generate a name for this computer player
			std::stringstream ss;
			ss << "Computer " << i+1;
			m_Players[i]=new AIPlayer(ss.str());

			// inform other players
			for (int j=0; j<4; j++) {
				if (i==j)
					continue;

				Human *hp=dynamic_cast<Human*>(m_Players[j]);
				if (hp)
					hp->getProtocol()->sendPlayerJoined(ss.str(), i);
			}
		}
	}

	Lockable::unlock();
}

void Room::randomizeTurnOrder() {
	Lockable::lock();

	// TODO: make this actually random
	m_TurnOrder[0]=0;
	m_TurnOrder[1]=3;
	m_TurnOrder[2]=2;
	m_TurnOrder[3]=1;

	// inform all human players
	for (int i=0; i<4; i++) {
		Human *hp=dynamic_cast<Human*>(m_Players[i]);
		if (hp)
			hp->getProtocol()->sendTurnOrder(m_TurnOrder);
	}

	Lockable::unlock();
}

// FIXME: this method really needs to be optimized...
void Room::tokenSelection() {
	Lockable::lock();

	// first alert all human players that its time to choose tokens
	for (int i=0; i<4; i++) {
		Human *hp=dynamic_cast<Human*>(m_Players[i]);
		if (hp)
			hp->getProtocol()->notify(Protocol::TokenSelectionBegin);
	}

	// used pieces
	std::vector<int> chosen;

	// now iterate over all players in order and have them choose a piece
	for (int i=0; i<4; i++) {
		Player *p=m_Players[m_TurnOrder[i]];

		// cast the player object
		AIPlayer *ai=dynamic_cast<AIPlayer*>(p);
		Human *hp=dynamic_cast<Human*>(p);

		// computer players just choose the next available piece
		int index=1;
		if (ai) {
			// don't overload clients with a barrage of packets
			sleep(2);

			for (int j=0; j<chosen.size(); j++) {
				if (chosen[j]==index) {
					index++;
					j=0;

					continue;
				}
			}

			std::cout << ai->getUsername() << " selected " << index << std::endl;
		}

		// human players need to choose the available ones
		else if (hp) {
			std::cout << "Letting " << hp->getUsername() << " know to choose a token\n";

			// tell the client to choose a token
			hp->getProtocol()->notify(Protocol::ChooseToken);

			// wait a maximum of 10 seconds for a reply
			Packet r;
			Packet::Result res=r.timedRead(hp->getProtocol()->getSocket(), 10, 0);
			if (res==Packet::NoError && r.byte()==GAME_SELECTED_TOK) {
				// we got a reply... assign him this piece
				index=r.byte();
			}

			// if we timed out, choose the next available piece
			else if (res==Packet::TimedOut) {
				// FIXME: great example of redundant code
				for (int j=0; j<chosen.size(); j++) {
					if (chosen[j]==index) {
						index++;
						j=0;

						continue;
					}
				}
			}

			// the client must have disconnected, replace him with a computer player
			else {
				// TODO
			}

			std::cout << hp->getUsername() << " selected " << index << std::endl;
		}

		// mark this piece as chosen
		chosen.push_back(index);

		// inform the other clients of this selection
		for (int j=0; j<4; j++) {
			Human *other=dynamic_cast<Human*>(m_Players[j]);
			if (other)
				other->getProtocol()->sendTokenSelected(i, index);
		}
	}

	// pause for a moment
	sleep(2);

	// inform all humans that token selection is over
	for (int i=0; i<4; i++) {
		Human *hp=dynamic_cast<Human*>(m_Players[i]);
		if (hp)
			hp->getProtocol()->notify(Protocol::TokenSelectionEnd);
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
