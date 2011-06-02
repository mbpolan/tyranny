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
// netmanager.cpp: implementation of the NetManager class.

#include "netmanager.h"
#include "protspec.h"

NetManager::NetManager(QObject *parent): QObject(parent) {
	m_Socket=new QTcpSocket(this);

	// connect socket signals
	connect(m_Socket, SIGNAL(connected()), this, SIGNAL(connected()));
	connect(m_Socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
	connect(m_Socket, SIGNAL(readyRead()), this, SLOT(onReadData()));
	connect(m_Socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
}

void NetManager::connectToServer(const QString &server, int port) {
	m_Socket->connectToHost(server, port);
}

void NetManager::disconnectFromServer() {
	m_Socket->disconnectFromHost();
}

void NetManager::sendAuthentication(const QString &username, const QString &password) {
	Packet p;
	p.addByte(AUTH_DATA);
	p.addString(username);
	p.addString(password);
	p.write(m_Socket);
}

void NetManager::onError(QAbstractSocket::SocketError error) {
	switch(error) {
		case QAbstractSocket::ConnectionRefusedError: emit networkError("Connection refused by peer."); break;
		case QAbstractSocket::HostNotFoundError: emit networkError("Host not found."); break;
		case QAbstractSocket::SocketTimeoutError: emit networkError("Connection timed out."); break;
		case QAbstractSocket::RemoteHostClosedError: emit networkError("Remote host closed the connection."); break;
		default: emit networkError("Unknown socket error."); break;
	}
}

void NetManager::onReadData() {
	while(m_Socket->bytesAvailable()) {
		// read the incoming data
		Packet p;
		while(p.read(m_Socket))
			parsePacket(p);
	}
}

void NetManager::parsePacket(Packet &p) {
	int header=p.byte();
	switch(header) {
		case AUTH_REQUEST: emit requireAuthentication(); break;
		case AUTH_ERROR: emit criticalError(p.string()); break;
		case AUTH_SUCCESS: emit networkMessage("Authentication successful."); break;

		default: qDebug() << "*** WARNING *** : Unknown packet header: " << header;
	}
}
