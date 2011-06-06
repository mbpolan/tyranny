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

		// user requested a password change
		case LB_CHANGEPASSWORD: handleChangePassword(p); break;

		// user requested his friends list
		case LB_FRIENDS_REQ: handleUserListRequest(p, false); break;

		// user requested his/her blocked user list
		case LB_BLOCKED_REQ: handleUserListRequest(p, true); break;

		// user sent an updated friend list
		case LB_FRIENDS_UPD: handleUserListUpdate(p, false); break;

		// user sent an updated blocked user list
		case LB_BLOCKED_UPD: handleUserListUpdate(p, true); break;

		default: std::cout << "** Unknown packet header: " << header << std::endl;
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
		Packet r;
		r.addByte(LB_STATISTICS);
		r.addUint32(points);
		r.addUint32(gamesPlayed);
		r.addUint32(won);
		r.addUint32(lost);
		r.write(m_Socket);
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
		Packet r;
		r.addByte(LB_USERPROFILE_REQ);
		r.addString(name);
		r.addString(email);
		r.addUint16(age);
		r.addString(bio);
		r.write(m_Socket);
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

void Protocol::handleChangePassword(Packet &p) {
	ConfigFile *cfg=ConfigFile::instance();

	// get the new password for the user
	std::string password=p.string();

	try {
		// connect to the database and update user's password
		DBMySQL db(cfg->getDBHost(), cfg->getDBPort(), cfg->getDBName());
		db.connect(cfg->getDBUser(), cfg->getDBPassword());
		db.updateUserPassword(m_User->getUsername(), password);
		db.disconnect();
	}

	catch (const DBMySQL::Exception &ex) {
		std::cout << "Unable to update user profile: " << ex.getMessage() << std::endl;
	}
}

void Protocol::handleUserListRequest(Packet &p, bool blocked) {
	ConfigFile *cfg=ConfigFile::instance();

	try {
		std::vector<std::string> friends;

		// connect to the database and get the user's friend list
		DBMySQL db(cfg->getDBHost(), cfg->getDBPort(), cfg->getDBName());
		db.connect(cfg->getDBUser(), cfg->getDBPassword());
		db.getUserList(m_User->getUsername(), friends, blocked);
		db.disconnect();

		// reply with this data
		Packet r;
		r.addByte((blocked ? LB_BLOCKED_REQ : LB_FRIENDS_REQ));
		r.addUint16(friends.size());

		// add each username
		for (int i=0; i<friends.size(); i++) {
			std::string username=friends[i];
			r.addString(username);
		}

		r.write(m_Socket);
	}

	catch (const DBMySQL::Exception &ex) {
		std::cout << "Unable to update user profile: " << ex.getMessage() << std::endl;
	}
}

void Protocol::handleUserListUpdate(Packet &p, bool blocked) {
	ConfigFile *cfg=ConfigFile::instance();

	// read the amount of usernames in the list
	int count=p.uint16();

	// read the usernames
	std::vector<std::string> list;
	for (int i=0; i<count; i++)
		list.push_back(p.string());

	try {
		// connect to the database and get the user's blocked user list
		DBMySQL db(cfg->getDBHost(), cfg->getDBPort(), cfg->getDBName());
		db.connect(cfg->getDBUser(), cfg->getDBPassword());
		db.updateUserList(m_User->getUsername(), list, blocked);
		db.disconnect();
	}

	catch (const DBMySQL::Exception &ex) {
		std::cout << "Unable to update user friend list: " << ex.getMessage() << std::endl;
	}
}
