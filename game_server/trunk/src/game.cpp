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
// game.cpp: implementation of the Game class.

#include <sys/epoll.h>

#include "game.h"
#include "human.h"

Game::Game(Room *room) {
	m_Room=room;
}

void Game::begin() {
	// determine turn order and send it to human players
	m_Room->randomizeTurnOrder();

	// create a polling descriptor for up for 4 client connections
	int pfd=epoll_create(4);

	// add the human players we have thus far to the poll fd
	m_Room->lock(Room::JoinMutex);

	static struct epoll_event e;
	for (int i=0; i<4; i++) {
		Human *hp=static_cast<Human*>(m_Room->getPlayers()[i]);

		if (hp) {
			e.events=EPOLLIN;
			e.data.fd=hp->getProtocol()->getSocket();
		}
	}
}
