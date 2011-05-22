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
// serversocket.cpp: implementation of the ServerSocket class

#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>

#include "serversocket.h"

ServerSocket::ServerSocket(const std::string &ip, int port) {
	m_IP=ip;
	m_Port=port;
	m_Socket=0;
}

void ServerSocket::bind() throw(ServerSocket::Exception) {
	m_Socket=socket(AF_INET, SOCK_STREAM, 0);
	if (m_Socket<0)
		throw ServerSocket::Exception("Unable to create socket.");
	
	// populate a server socket struct
	struct sockaddr_in sa;
	sa.sin_family=AF_INET;
	sa.sin_port=htons(m_Port);
	sa.sin_addr.s_addr=inet_addr(m_IP.c_str());
	memset(&sa.sin_zero, 0, sizeof(sa.sin_zero));
	
	if (::bind(m_Socket, (struct sockaddr*) &sa, sizeof(struct sockaddr_in))<0)
		throw ServerSocket::Exception("Unable to bind to IP address.");
}
		
void ServerSocket::listen() throw(ServerSocket::Exception) {
	// try listening on the socket (backlog of 10)
	if (::listen(m_Socket, 10)<0)
		throw ServerSocket::Exception("Unable to listen on socket.");
}

ServerSocket::Client* ServerSocket::accept() {
	struct sockaddr_in cl;
	socklen_t len=sizeof(cl);
	int s=::accept(m_Socket, (struct sockaddr*) &cl, &len);

	if (s>0)
		return new Client(inet_ntoa(cl.sin_addr), ntohs(cl.sin_port), s);
	else
		return NULL;
}
