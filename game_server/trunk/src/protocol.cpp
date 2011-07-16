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
// protocol.cpp: implementation of the Protocol class.

#include "packet.h"
#include "protocol.h"
#include "protspec.h"

Protocol::Protocol(int socket) {
	m_Socket=socket;
}

void Protocol::sendPlayerJoined(const std::string &username, int index) {
	Packet p;
	p.addByte(GAME_PLAYER_JOINED);
	p.addString(username);
	p.addByte(index);
	p.write(m_Socket);
}

void Protocol::sendPlayerQuit(int index) {
	Packet p;
	p.addByte(GAME_PLAYER_QUIT);
	p.addByte(index);
	p.write(m_Socket);
}

void Protocol::sendStartControl() {
	Packet p;
	p.addByte(GMRM_START_WAIT);
	p.write(m_Socket);
}

void Protocol::sendTurnOrder(const std::vector<int> &order) {
	Packet p;
	p.addByte(GAME_TURN_ORDER);

	for (int i=0; i<4; i++)
		p.addByte(order[i]);

	p.write(m_Socket);
}
