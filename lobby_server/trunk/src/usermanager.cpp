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
// usermanager.cpp: implementation of the UserManager class.

#include "dbmysql.h"
#include "usermanager.h"

// global instance of the user manager
UserManager *g_Manager=NULL;

UserManager::UserManager() {
	pthread_mutex_init(&m_Mutex, NULL);

	g_Manager=this;
}

UserManager::~UserManager() {
	pthread_mutex_destroy(&m_Mutex);
}

UserManager* UserManager::instance() {
	return g_Manager;
}

void UserManager::addUser(User *user) {
	pthread_mutex_lock(&m_Mutex);

	// hash the user by his username
	m_UserMap[user->getUsername()]=user;

	// now let all the other users know that this user has logged in
	for (std::map<std::string, User*>::iterator it=m_UserMap.begin(); it!=m_UserMap.end(); ++it) {
		User *other=(*it).second;

		// check if this other user is being blocked by the newly logged in user
		bool ignore=user->isBlocking(other->getUsername());

		// ignore the other user is not being blocked, inform him that we just logged in
		if (!ignore) {
			Protocol::UserStatus relation=Protocol::UserNone;
			if (other->isBlocking(user->getUsername())) relation=Protocol::UserBlocked;
			else if (other->isFriendsWith(user->getUsername())) relation=Protocol::UserFriend;

			other->getProtocol()->sendUserLoggedIn(user, relation);
		}

		// now check if the other user is blocking THIS user
		ignore=false;
		for (int i=0; i<other->getBlockedList().size(); i++) {
			if (other->getBlockedList()[i]==user->getUsername())
				ignore=true;
		}

		// finally if we are not being blocked, inform us that this other user is online
		if (other!=user && !ignore) {
			Protocol::UserStatus relation=Protocol::UserNone;
			if (user->isBlocking(other->getUsername())) relation=Protocol::UserBlocked;
			else if (user->isFriendsWith(other->getUsername())) relation=Protocol::UserFriend;
			user->getProtocol()->sendUserLoggedIn(other, relation);
		}
	}

	// flag the user as online
	try {
		pDBMySQL db=DBMySQL::synthesize();
		db->flagUserOnline(user->getUsername(), true);
		db->disconnect();
	}

	catch (const DBMySQL::Exception &ex) {
		std::cout << "Unable to flag user as online: " << ex.getMessage() << std::endl;
	}

	pthread_mutex_unlock(&m_Mutex);
}

void UserManager::removeUser(User *user) {
	pthread_mutex_lock(&m_Mutex);

	// remove the user from the hash map
	m_UserMap.erase(user->getUsername());

	// send all other users a message that this user logged out
	for (std::map<std::string, User*>::iterator it=m_UserMap.begin(); it!=m_UserMap.end(); ++it) {
		User *other=(*it).second;
		other->getProtocol()->sendUserLoggedOut(user);
	}

	// flag the user as offline
	try {
		pDBMySQL db=DBMySQL::synthesize();
		db->flagUserOnline(user->getUsername(), false);
		db->disconnect();
	}

	catch (const DBMySQL::Exception &ex) {
		std::cout << "Unable to flag user as online: " << ex.getMessage() << std::endl;
	}

	pthread_mutex_unlock(&m_Mutex);
}

void UserManager::broadcastChatMessage(const std::string &user, const std::string &message) {
	pthread_mutex_lock(&m_Mutex);

	User *sender=m_UserMap[user];

	for (std::map<std::string, User*>::iterator it=m_UserMap.begin(); it!=m_UserMap.end(); ++it) {
		User *other=(*it).second;

		// if this user is on the target user's blocked list, don't sent him the chat message
		bool ignore=false;
		for (int i=0; i<other->getBlockedList().size(); i++) {
			// make sure the other user is not blocking the sender
			if (other->getBlockedList()[i]==user)
				ignore=true;
		}

		// if the sender is blocking the recipient, don't send him the message either
		for (int i=0; i<sender->getBlockedList().size(); i++) {
			if (sender->getBlockedList()[i]==other->getUsername())
				ignore=true;
		}

		// if all checks out, send the recepient the chat message
		if (!ignore)
			other->getProtocol()->sendChatMessage(user, message);
	}

	pthread_mutex_unlock(&m_Mutex);
}

void UserManager::sendUserStatusUpdate(const std::string &user, const std::string &target, bool online) {
	pthread_mutex_lock(&m_Mutex);

	// see if the user is online at this point
	User *toWhom=m_UserMap[user];
	User *targetUser=m_UserMap[target];
	if (toWhom && targetUser) {
		if (online) {
			Protocol::UserStatus relation=Protocol::UserNone;
			if (toWhom->isBlocking(targetUser->getUsername())) relation=Protocol::UserBlocked;
			else if (toWhom->isFriendsWith(targetUser->getUsername())) relation=Protocol::UserFriend;
			toWhom->getProtocol()->sendUserLoggedIn(targetUser, relation);
		}

		else
			toWhom->getProtocol()->sendUserLoggedOut(targetUser);
	}

	pthread_mutex_unlock(&m_Mutex);
}
