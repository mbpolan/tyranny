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
// clientsocket.cpp: implementation of the ClientSocket class.

#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sstream>

#include "clientsocket.h"

ClientSocket::ClientSocket() {
	m_Socket=-1;
}

void ClientSocket::connect(const std::string &host, int port) throw(ClientSocket::Exception) {
	// create the socket itself
	m_Socket=socket(AF_INET, SOCK_STREAM, 0);
	if (m_Socket<0)
		throw ClientSocket::Exception("Unable to create client socket.");

	struct sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=inet_addr(host.c_str());
	addr.sin_port=htons(port);
	memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

	if (::connect(m_Socket, (struct sockaddr*) &addr, sizeof(struct sockaddr_in))<0)
		throw ClientSocket::Exception("Unable to connect to host.");
}

void ClientSocket::disconnect() throw (ClientSocket::Exception) {
	if (m_Socket==-1)
		throw ClientSocket::Exception("There is no active socket to disconnect.");

	close(m_Socket);
	m_Socket=-1;
}
