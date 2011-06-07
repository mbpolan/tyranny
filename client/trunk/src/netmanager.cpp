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

#include <QStringList>

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

void NetManager::sendChatMessage(const QString &message) {
	Packet p;
	p.addByte(LB_CHATMESSAGE);
	p.addString(message);
	p.write(m_Socket);
}

void NetManager::requestFriendList() {
	Packet p;
	p.addByte(LB_FRIENDS_REQ);
	p.write(m_Socket);
}

void NetManager::sendFriendListUpdate(const QStringList &list) {
	Packet p;
	p.addByte(LB_FRIENDS_UPD);
	p.addUint16(list.size());

	// add each username
	for (int i=0; i<list.size(); i++)
	    p.addString(list[i]);

	p.write(m_Socket);
}

void NetManager::requestBlockedList() {
	Packet p;
	p.addByte(LB_BLOCKED_REQ);
	p.write(m_Socket);
}

void NetManager::sendBlockedListUpdate(const QStringList &list) {
	Packet p;
	p.addByte(LB_BLOCKED_UPD);
	p.addUint16(list.size());

	// add each username
	for (int i=0; i<list.size(); i++)
		p.addString(list[i]);

	p.write(m_Socket);
}

void NetManager::requestStatistics() {
	Packet p;
	p.addByte(LB_STATISTICS);
	p.write(m_Socket);
}

void NetManager::requestUserProfile() {
	Packet p;
	p.addByte(LB_USERPROFILE_REQ);
	p.write(m_Socket);
}

void NetManager::sendUserProfileUpdate(const QString &name, const QString &email, int age, const QString &bio) {
	Packet p;
	p.addByte(LB_USERPROFILE_UPD);
	p.addString(name);
	p.addString(email);
	p.addUint16(age);
	p.addString(bio);
	p.write(m_Socket);
}

void NetManager::sendPasswordChange(const QString &password) {
	Packet p;
	p.addByte(LB_CHANGEPASSWORD);
	p.addString(password);
	p.write(m_Socket);
}

void NetManager::sendUserRequest(const QString &username, const UserRequest &list) {
	Packet p;
	p.addByte(LB_USERREQUEST);
	p.addString(username);
	p.addByte((list==NetManager::FriendRequest ? REQ_FRIENDS : REQ_BLOCKED));
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
	uint8_t header=p.byte();
	switch(header) {
		case AUTH_REQUEST: emit requireAuthentication(); break;
		case AUTH_ERROR: emit serverError(p.string()); break;
		case AUTH_SUCCESS: emit statusMessage("Welcome to "+p.string()); break;

		case LB_USERIN: handleUserLoggedIn(p); break;
		case LB_USEROUT: emit userLoggedOut(p.string()); break;
		case LB_CHATMESSAGE: handleLobbyChatMessage(p); break;
		case LB_STATISTICS: handleStatistics(p); break;
		case LB_USERPROFILE_REQ: handleUserProfileRequest(p); break;
		case LB_FRIENDS_REQ: handleFriendListRequest(p); break;
		case LB_BLOCKED_REQ: handleBlockedListRequest(p); break;

		case MSG_INFO: emit serverInfo(p.string()); break;
		case MSG_ERROR: emit serverError(p.string()); break;

		default: qDebug() << "*** WARNING *** : Unknown packet header: " << header; break;
	}
}

void NetManager::handleUserLoggedIn(Packet &p) {
	// get the username
	QString username=p.string();

	// get the user's status
	char status=p.byte();

	// translate the status into an enum
	NetManager::UserStatus st;
	switch(status) {
		default: st=NetManager::UserNone; break;
		case USER_BLOCKED: st=NetManager::UserBlocked; break;
		case USER_FRIEND: st=NetManager::UserFriend; break;
	}

	emit userLoggedIn(username, st);
}

void NetManager::handleLobbyChatMessage(Packet &p) {
	// get the information from the packet
	QString sender=p.string();
	QString message=p.string();

	emit lobbyChatMessage(sender, message);
}

void NetManager::handleStatistics(Packet &p) {
	// get the pertinent data from the packet
	int points=p.uint32();
	int games=p.uint32();
	int won=p.uint32();
	int lost=p.uint32();

	emit userStatistics(points, games, won, lost);
}

void NetManager::handleUserProfileRequest(Packet &p) {
	// get the pertinent data
	QString name=p.string();
	QString email=p.string();
	int age=p.uint16();
	QString bio=p.string();

	emit userProfile(name, email, age, bio);
}

void NetManager::handleFriendListRequest(Packet &p) {
	// get the amount of usernames
	int count=p.uint16();

	// read each username
	QStringList list;
	for (int i=0; i<count; i++)
	    list.append(p.string());

	emit userFriendList(list);
}

void NetManager::handleBlockedListRequest(Packet &p) {
    // get the amount of usernames
    int count=p.uint16();

    // read each username
    QStringList list;
    for (int i=0; i<count; i++)
	  list.append(p.string());

    emit userBlockedList(list);
}
