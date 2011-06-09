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
// serverpool.cpp: implementation of the ServerPool class.

#include "serverpool.h"

// globals
ServerPool *g_ServerPool=NULL;

ServerPool::ServerPool() {
	g_ServerPool=this;
}

ServerPool::~ServerPool() {

}

ServerPool* ServerPool::instance() {
	return g_ServerPool;
}

void ServerPool::addGameServer(const std::string &id, const std::string &host, int port) {
	m_Servers[id]=new ServerPool::GameServer(id, host, port);
}

void ServerPool::getGameServer(const std::string &id, std::string &host, int &port) {
	GameServer *server=m_Servers[id];
	host=server->getHost();
	port=server->getPort();
}

void ServerPool::selectGameServer(std::string &host, int &port) {
	// just grab the first server for now
	GameServer *server=(*m_Servers.begin()).second;
	host=server->getHost();
	port=server->getPort();
}
