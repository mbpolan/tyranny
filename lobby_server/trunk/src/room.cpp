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

Room::Room(int gid, const std::string &owner, const Type &type, const std::string &password, bool friendsOnly) {
	m_Gid=gid;
	m_Type=type;
	m_Password=password;
	m_FriendsOnly=friendsOnly;
	m_Owner=owner;
	m_Rules=Rules(0, 0, 0, false, Rules::RandomToPlayers);
}

void Room::setConnectionInfo(const std::string &host, int port) {
	m_Host=host;
	m_Port=port;
}

void Room::getConnectionInfo(std::string &host, int &port) {
	host=m_Host;
	port=m_Port;
}

void Room::removePlayer(const std::string &username) {
	std::vector<std::string> nlist;

	for (int i=0; i<m_Players.size(); i++) {
		if (m_Players[i]!=username)
			nlist.push_back(m_Players[i]);
	}

	m_Players=nlist;
}
