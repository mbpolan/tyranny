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
// gameprotocol.cpp: implementation of the GameProtocol class.

#include <QStatusBar>

#include "gameprotocol.h"
#include "packet.h"
#include "protspec.h"

GameProtocol::GameProtocol(int gid, const QString &username, QObject *parent): QObject(parent) {
	m_Gid=gid;
	m_Username=username;
}

void GameProtocol::connectToServer(const QString &host, int port) {
	m_Socket=new QTcpSocket(this);

	// connect signals
	connect(m_Socket, SIGNAL(connected()), this, SLOT(onConnected()));
	connect(m_Socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
	connect(m_Socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
	connect(m_Socket, SIGNAL(readyRead()), this, SLOT(onDataReady()));

	m_Socket->connectToHost(host, port);
}

void GameProtocol::disconnectFromServer() {
	m_Socket->disconnectFromHost();
}

void GameProtocol::onConnected() {
	Packet p;
	p.addByte(CONN_CLIENT);
	p.addString(m_Username);
	p.addUint32(m_Gid);
	p.write(m_Socket);

	emit connected();
}

void GameProtocol::onDisconnected() {
	emit disconnected();
}

void GameProtocol::onError(const QAbstractSocket::SocketError &error) {
	switch(error) {
		case QAbstractSocket::ConnectionRefusedError: emit networkError("Connection refused by peer."); break;
		case QAbstractSocket::HostNotFoundError: emit networkError("Host not found."); break;
		case QAbstractSocket::SocketTimeoutError: emit networkError("Connection timed out."); break;
		case QAbstractSocket::RemoteHostClosedError: emit networkError("Remote host closed the connection."); break;
		default: emit networkError("Unknown socket error."); break;
	}
}

void GameProtocol::onDataReady() {

}

void GameProtocol::parsePacket(Packet &p) {

}
