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

		// user sent a request to add another user to friends/blocked list
		case LB_USERREQUEST: handleUserRequest(p); break;

		default: std::cout << "** Unknown packet header: " << header << std::endl; break;
	}
}

void Protocol::handleUserChatMessage(Packet &p) {
	// get the message contents
	std::string message=p.string();

	// let the user manager handle this action
	UserManager::instance()->broadcastChatMessage(m_User->getUsername(), message);
}

void Protocol::handleStatistics(Packet &p) {
	int points, gamesPlayed, won, lost;

	try {
		// connect to the database and get the statistics
		pDBMySQL db=DBMySQL::synthesize();
		db->getUserStatistics(m_User->getUsername(), points, gamesPlayed, won, lost);
		db->disconnect();

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
	std::string name, email, bio;
	int age;

	try {
		// connect to the database and get the profile data
		pDBMySQL db=DBMySQL::synthesize();
		db->getUserProfile(m_User->getUsername(), name, email, age, bio);
		db->disconnect();

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
	// cache the packet data
	std::string name=p.string();
	std::string email=p.string();
	int age=p.uint16();
	std::string bio=p.string();

	try {
		// connect to the database and update the profile data
		pDBMySQL db=DBMySQL::synthesize();
		db->updateUserProfile(m_User->getUsername(), name, email, age, bio);
		db->disconnect();
	}

	catch (const DBMySQL::Exception &ex) {
		std::cout << "Unable to update user profile: " << ex.getMessage() << std::endl;
	}
}

void Protocol::handleChangePassword(Packet &p) {
	// get the new password for the user
	std::string password=p.string();

	try {
		// connect to the database and update user's password
		pDBMySQL db=DBMySQL::synthesize();
		db->updateUserPassword(m_User->getUsername(), password);
		db->disconnect();
	}

	catch (const DBMySQL::Exception &ex) {
		std::cout << "Unable to update user profile: " << ex.getMessage() << std::endl;
	}
}

void Protocol::handleUserListRequest(Packet &p, bool blocked) {
	try {
		std::vector<std::string> friends;

		// connect to the database and get the user's friend list
		pDBMySQL db=DBMySQL::synthesize();
		db->getUserList(m_User->getUsername(), friends, blocked);
		db->disconnect();

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
	// read the amount of usernames in the list
	int count=p.uint16();

	// read the usernames
	std::vector<std::string> list;
	for (int i=0; i<count; i++)
		list.push_back(p.string());

	try {
		// connect to the database and get the appropriate list
		pDBMySQL db=DBMySQL::synthesize();
		db->updateUserList(m_User->getUsername(), list, blocked);
		db->disconnect();

		// store the old blocked list
		std::vector<std::string> oldBlocked=m_User->getBlockedList();

		// also update the user object
		if (blocked)
			m_User->setBlockedList(list);
		else
			m_User->setFriendList(list);

		// now find out which users have been removed from the blocked list, and if
		// they are online, inform them that we are online too
		for (int i=0; i<oldBlocked.size(); i++) {
			bool removed=true;

			for (int j=0; j<list.size(); j++) {
				if (oldBlocked[i]==list[j])
					removed=false;
			}

			// if this user was removed, alert him of our presence
			if (removed)
				UserManager::instance()->sendUserStatusUpdate(oldBlocked[i], m_User->getUsername(), true);
		}
	}

	catch (const DBMySQL::Exception &ex) {
		std::cout << "Unable to update user friend list: " << ex.getMessage() << std::endl;
	}
}

void Protocol::handleUserRequest(Packet &p) {
	// read the user and list target
	std::string username=p.string();
	char list=p.byte();

	// verify the user isn't trying to add himself
	if (username==m_User->getUsername()) {
		Packet r;
		r.addByte(MSG_ERROR);
		r.addString("You cannot add yourself to a friend or blocked list.");
		r.write(m_Socket);

		return;
	}

	try {
		// connect to the database and add the user to the list
		pDBMySQL db=DBMySQL::synthesize();
		DBMySQL::RequestResult res=db->addUserToList(m_User->getUsername(), username, (list==REQ_FRIENDS ? false : true));
		db->disconnect();

		// determine the results
		if (res==DBMySQL::NoError) {
			std::string msg="Successfully added user to ";

			// complete the message string and update the user object
			if (list==REQ_FRIENDS) {
				msg+="friends list.";

				std::vector<std::string> friends=m_User->getFriendList();
				friends.push_back(username);
				m_User->setFriendList(friends);
			}

			else {
				msg+="blocked list.";

				std::vector<std::string> blocked=m_User->getBlockedList();
				blocked.push_back(username);
				m_User->setBlockedList(blocked);

				// make sure the blocked user can no longer see us if he's online right now
				UserManager::instance()->sendUserStatusUpdate(username, m_User->getUsername(), false);
			}

			Packet r;
			r.addByte(MSG_INFO);
			r.addString(msg);
			r.write(m_Socket);
		}

		else if (res==DBMySQL::DuplicateEntry) {
			Packet r;
			r.addByte(MSG_ERROR);
			r.addString("The given user already exists in one of your lists.");
			r.write(m_Socket);
		}
	}

	catch (const DBMySQL::Exception &ex) {
		std::cout << "Unable to update user friend list: " << ex.getMessage() << std::endl;
	}
}
