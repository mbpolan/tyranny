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

#include "configfile.h"
#include "dbmysql.h"
#include "packet.h"
#include "protocol.h"
#include "protspec.h"
#include "user.h"
#include "usermanager.h"

Protocol::Protocol(int socket): m_Socket(socket) {
	m_User=NULL;
}

void Protocol::communicationLoop() {
	Packet p;
	Packet::Result res;

	do {
		// read a single packet
		res=p.read(m_Socket);
		parsePacket(p);

	} while(res!=Packet::DataCorrupt && res!=Packet::Disconnected);
}

void Protocol::sendUserLoggedIn(User *other) {
	Packet p;
	p.addByte(LB_USERIN);
	p.addString(other->getUsername());
	p.write(m_Socket);
}

void Protocol::sendUserLoggedOut(User *other) {
	Packet p;
	p.addByte(LB_USEROUT);
	p.addString(other->getUsername());
	p.write(m_Socket);
}

void Protocol::sendChatMessage(const std::string &user, const std::string &message) {
	Packet p;
	p.addByte(LB_CHATMESSAGE);
	p.addString(user);
	p.addString(message);
	p.write(m_Socket);
}

void Protocol::parsePacket(Packet &p) {
	uint8_t header=p.byte();
	switch(header) {
		// user sent a chat message
		case LB_CHATMESSAGE: handleUserChatMessage(p); break;

		// user requested his/her statistics
		case LB_STATISTICS: handleStatistics(p); break;

		// user requested his/her profile
		case LB_USERPROFILE_REQ: handleUserProfileRequest(p); break;

		// user sent an updated profile to be stored
		case LB_USERPROFILE_UPD: handleUserProfileUpdate(p); break;

		default: break;
	}
}

void Protocol::handleUserChatMessage(Packet &p) {
	// get the message contents
	std::string message=p.string();

	// let the user manager handle this action
	UserManager::instance()->broadcastChatMessage(m_User->getUsername(), message);
}

void Protocol::handleStatistics(Packet &p) {
	ConfigFile *cfg=ConfigFile::instance();
	int points, gamesPlayed, won, lost;

	try {
		// connect to the database and get the statistics
		DBMySQL db(cfg->getDBHost(), cfg->getDBPort(), cfg->getDBName());
		db.connect(cfg->getDBUser(), cfg->getDBPassword());
		db.getUserStatistics(m_User->getUsername(), points, gamesPlayed, won, lost);
		db.disconnect();

		// reply to the client
		Packet p;
		p.addByte(LB_STATISTICS);
		p.addUint32(points);
		p.addUint32(gamesPlayed);
		p.addUint32(won);
		p.addUint32(lost);
		p.write(m_Socket);
	}

	catch (const DBMySQL::Exception &ex) {
		std::cout << "Unable to get statistics from database: " << ex.getMessage() << std::endl;
	}
}

void Protocol::handleUserProfileRequest(Packet &p) {
	ConfigFile *cfg=ConfigFile::instance();
	std::string name, email, bio;
	int age;

	try {
		// connect to the database and get the profile data
		DBMySQL db(cfg->getDBHost(), cfg->getDBPort(), cfg->getDBName());
		db.connect(cfg->getDBUser(), cfg->getDBPassword());
		db.getUserProfile(m_User->getUsername(), name, email, age, bio);
		db.disconnect();

		// reply to the client
		Packet p;
		p.addByte(LB_USERPROFILE_REQ);
		p.addString(name);
		p.addString(email);
		p.addUint16(age);
		p.addString(bio);
		p.write(m_Socket);
	}

	catch (const DBMySQL::Exception &ex) {
		std::cout << "Unable to get user profile from database: " << ex.getMessage() << std::endl;
	}
}

void Protocol::handleUserProfileUpdate(Packet &p) {
	ConfigFile *cfg=ConfigFile::instance();

	// cache the packet data
	std::string name=p.string();
	std::string email=p.string();
	int age=p.uint16();
	std::string bio=p.string();

	try {
		// connect to the database and update the profile data
		DBMySQL db(cfg->getDBHost(), cfg->getDBPort(), cfg->getDBName());
		db.connect(cfg->getDBUser(), cfg->getDBPassword());
		db.updateUserProfile(m_User->getUsername(), name, email, age, bio);
		db.disconnect();
	}

	catch (const DBMySQL::Exception &ex) {
		std::cout << "Unable to update user profile: " << ex.getMessage() << std::endl;
	}
}
