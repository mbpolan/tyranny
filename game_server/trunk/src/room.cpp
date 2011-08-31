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

#include <sys/ioctl.h>
#include <sys/select.h>
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
	m_Phase=Init;
	m_Players=std::vector<Player*>(4);
	m_ChosenPieces=std::vector<int>(4);
	m_TurnOrder=Util::generateTurnOrder();

	// initialize the players vector to be all NULL by default, and
	// set all chosen pieces to be -1 (not claimed)
	for (int i=0; i<4; i++) {
		m_Players[i]=NULL;
		m_ChosenPieces[i]=-1;
	}
}

void Room::setRules(const Room::Rules &rules) {
	Lockable::lock();

	m_Rules=rules;

	Lockable::unlock();
}

Room::Rules Room::getRules() {
	lock();

	Room::Rules rules=m_Rules;

	Lockable::unlock();

	return rules;
}

void Room::setPhase(const Room::Phase &phase) {
	lock();

	m_Phase=phase;

	unlock();
}

Room::Phase Room::getPhase() {
	lock();

	Room::Phase phase=m_Phase;

	unlock();
	return phase;
}

bool Room::allowNewPlayers(std::string &result) {
	lock();

	// by default we assume a player can join
	bool yes=true;

	// if this room is not in a proper phase, reject new players
	if (m_Phase==Init || m_Phase==AwaitOwner) {
		result="This room is not yet ready";
		yes=false;
	}

	// if we are maxed out of slots, also reject
	if (m_NumHumans==4) {
		result="Too many players";
		yes=false;
	}

	unlock();

	return yes;
}

void Room::addOwner(Player *player) {
	lock();

	m_Players[0]=player;

	unlock();
}

void Room::queuePlayer(Player *player) {
	lock();

	m_Queued.push(player);
	m_FDBuffer.awake();

	unlock();
}

void Room::begin() {
	lock();

	// first we flag the new phase
	m_Phase=AwaitMorePlayers;

	// and tell the owner he has control
	Human *owner=static_cast<Human*>(m_Players[0]);
	owner->getProtocol()->sendStartControl();
	owner->getProtocol()->sendPlayerJoined(owner->getUsername(), 0);

	// and add the owner's fd
	m_FDBuffer.addSocket(owner->getProtocol()->getSocket());

	// start the relay loop
	bool loop=true;
	while(loop) {
		unlock();

		// see if any clients have sent data
		FDBuffer::WaitCode code=m_FDBuffer.poll();

		lock();

		// cache active sockets
		m_ActiveSockets=mapToPlayers(m_FDBuffer.getActiveSockets());

		// purge any disconnected players now
		handleDisconnectedPlayers();

		// take action based on what happened
		switch(m_Phase) {
			// we are waiting for the owner to start the room
			case AwaitMorePlayers: handleAwaitMorePlayers(code); break;

			// decide turn orders for current players
			case FindTurnOrder: handleFindTurnOrder(code); break;

			// end the game room
			case Terminating: loop=false; break;

			// this shouldn't happen
			default: break;
		}

		// we also need to accept queued players
		handleQueuedPlayers();
	}

	// disconnect all clients
	for (int i=0; i<4; i++) {
		Human *hp=dynamic_cast<Human*>(m_Players[i]);
		if (hp)
			close(hp->getProtocol()->getSocket());

		delete hp;
		m_Players[i]=NULL;
	}
}

void Room::removePlayer(Player *player, bool replace) {
	for (int i=0; i<4; i++) {
		// is this the player?
		if (m_Players[i]==player) {
			m_Players[i]=NULL;

			// should we replace the player?
			if (replace) {
				std::stringstream ss;
				ss << "Computer " << i+1;
				m_Players[i]=new AIPlayer(ss.str());
			}

			// if this is a human player, close the socket as well
			Human *hp=dynamic_cast<Human*>(player);
			if (hp) {
				m_FDBuffer.removeSocket(hp->getProtocol()->getSocket());
				close(hp->getProtocol()->getSocket());

				m_NumHumans--;
			}

			// alert everyone that this player is no longer around
			for (int j=0; j<4; j++) {
				if (i==j)
					continue;

				Human *other=dynamic_cast<Human*>(m_Players[j]);
				if (other) {
					other->getProtocol()->sendPlayerQuit(i);

					if (m_Players[i])
						other->getProtocol()->sendPlayerJoined(m_Players[i]->getUsername(), i);
				}
			}

			// and finally delete this object
			delete player;
		}
	}
}

std::vector<Human*> Room::mapToPlayers(const std::vector<int> &fds) {
	std::vector<Human*> players;

	for (int i=0; i<fds.size(); i++) {
		for (int j=0; j<m_Players.size(); j++) {
			Human *hp=dynamic_cast<Human*> (m_Players[j]);
			if (hp && hp->getProtocol()->getSocket()==fds[i])
				players.push_back(hp);
		}
	}

	return players;
}

void Room::broadcastPlayerJoined(Player *player, int index) {
	// see if this a human player
	Human *hp=dynamic_cast<Human*>(player);

	for (int i=0; i<4; i++) {
		Human *other=dynamic_cast<Human*>(m_Players[i]);
		if (other) {
			other->getProtocol()->sendPlayerJoined(player->getUsername(), index);

			// also send this player's name to the newly connected client
			if (hp && other!=hp)
				hp->getProtocol()->sendPlayerJoined(other->getUsername(), i);
		}
	}
}

void Room::broadcastTurnOrder() {
	for (int i=0; i<4; i++) {
		Human *hp=dynamic_cast<Human*>(m_Players[i]);
		if (hp)
			hp->getProtocol()->sendTurnOrder(m_TurnOrder);
	}
}

void Room::broadcastNotification(const Protocol::Notification &note) {
	for (int i=0; i<4; i++) {
		Human *hp=dynamic_cast<Human*>(m_Players[i]);
		if (hp)
			hp->getProtocol()->notify(note);
	}
}

void Room::broadcastTokenChosen(int player, int piece) {
	for (int i=0; i<4; i++) {
		Human *hp=dynamic_cast<Human*>(m_Players[i]);
		if (hp)
			hp->getProtocol()->sendTokenSelected(player, piece);
	}
}

void Room::assignAI() {
	for (int i=0; i<4; i++) {
		if (!m_Players[i]) {
			std::stringstream ss;
			ss << "Computer " << i+1;

			// create a new computer player object
			AIPlayer *ai=new AIPlayer(ss.str());
			m_Players[i]=ai;

			// and let everyone know it "joined"
			broadcastPlayerJoined(ai, i);
		}
	}
}

void Room::handleDisconnectedPlayers() {
	for (int i=0; i<m_ActiveSockets.size(); i++) {
		Human *hp=m_ActiveSockets[i];
		int n;

		if (hp && ioctl(hp->getProtocol()->getSocket(), FIONREAD, &n)>-1) {
			// determine if this client disconnected
			if (n==0) {
				// certain phases are exceptional
				if (m_Phase==AwaitMorePlayers && hp->getUsername()==m_Players[0]->getUsername())
					m_Phase=Terminating;

				// otherwise remove this player from the vector and fd buffer
				removePlayer(hp, false);
				m_ActiveSockets.erase(m_ActiveSockets.begin()+i);
			}
		}
	}
}

void Room::handleQueuedPlayers() {
	while(!m_Queued.empty()) {
		// this cannot fail... riiight? :P
		Human *hp=static_cast<Human*>(m_Queued.front());
		m_Queued.pop();

		// verify we have a slot
		if (m_NumHumans==4) {
			close(hp->getProtocol()->getSocket());
			delete hp;

			continue;
		}

		// add him into the vector
		for (int i=0; i<4; i++) {
			if (!m_Players[i]) {
				m_Players[i]=hp;
				m_NumHumans++;

				// add his socket to the buffer
				m_FDBuffer.addSocket(hp->getProtocol()->getSocket());

				broadcastPlayerJoined(hp, i);

				break;
			}
		}

		// TODO: if the game is in progress, assign this player a piece
	}
}

void Room::handleAwaitMorePlayers(const FDBuffer::WaitCode &fdc) {
	// the only thing we care about is socket action
	if (fdc!=FDBuffer::DataReady)
		return;

	for (int i=0; i<m_ActiveSockets.size(); i++) {
		Packet p;
		p.read(m_ActiveSockets[i]->getProtocol()->getSocket());

		// if the owner replied, see if he wants to start the game room
		if (m_ActiveSockets[i]->getUsername()==m_Players[0]->getUsername() && p.byte()==GMRM_BEGIN_GAME) {
				// now fill all empty slots with computer players
				assignAI();

				// inform everyone of the turn order
				broadcastTurnOrder();

				m_Phase=FindTurnOrder;
		}
	}
}

void Room::handleFindTurnOrder(const FDBuffer::WaitCode &fdc) {
	// nothing to do but wait for the client's animations to complete
	if (fdc==FDBuffer::TimeExpired) {
		// first tell all clients that token selection has begun
		broadcastNotification(Protocol::TokenSelectionBegin);

		// -1 means that the first player has yet to receive token selection control
		m_CurPlayer=-1;

		// let clients breathe and set the next phase
		m_FDBuffer.setExpireTime(2);
		m_Phase=TokenSelection;
	}
}

void Room::handleTokenSelection(const FDBuffer::WaitCode &fdc) {

}
