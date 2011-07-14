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

#include "room.h"

Room::Room(int gid, const std::string &owner) {
	m_Gid=gid;
	m_Owner=owner;
	m_Rules=Rules(0, 0, 0, false, Rules::RandomToPlayers);
	m_Players=std::vector<Player*>(4);

	for (int i=0; i<4; i++)
		m_Players[i]=NULL;
}

void Room::setRules(const Room::Rules &rules) {
	lock();
	m_Rules=rules;
	unlock();
}

Room::Rules Room::getRules() {
	Room::Rules rules;

	lock();
	rules=m_Rules;
	unlock();

	return rules;
}

void Room::addPlayer(Player *player) {
	lock();

	// find an empty slot
	for (int i=0; i<4; i++) {
		if (!m_Players[i]) {
			m_Players[i]=player;
			break;
		}
	}

	unlock();
}

void Room::removePlayer(const Player *player) {
	lock();

	for (int i=0; i<4; i++) {
		if (m_Players[i]==player)
			m_Players[i]=NULL;
	}

	unlock();
}

std::vector<Player*> Room::getPlayers() {
	std::vector<Player*> list;

	lock();
	list=m_Players;
	unlock();

	return list;
}

int Room::getNumPlayers() {
	lock();

	int count=0;

	for (int i=0; i<4; i++) {
		if (m_Players[i])
			count++;
	}

	unlock();

	return count;
}
