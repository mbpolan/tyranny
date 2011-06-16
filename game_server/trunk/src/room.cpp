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
	m_Players.push_back(player);
	unlock();
}

void Room::removePlayer(const Player *player) {
	lock();

	std::vector<Player*> nlist;

	for (int i=0; i<m_Players.size(); i++) {
		if (m_Players[i]!=player)
			nlist.push_back(m_Players[i]);
	}

	m_Players=nlist;
	unlock();
}

std::vector<Player*> Room::getPlayers() {
	std::vector<Player*> list;

	lock();
	list=m_Players;
	unlock();

	return list;
}
