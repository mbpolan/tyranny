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
		other->getProtocol()->sendUserLoggedIn(user);

		// also update the logged in user with the current users online now
		if (other!=user)
			user->getProtocol()->sendUserLoggedIn(other);
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

	pthread_mutex_unlock(&m_Mutex);
}

void UserManager::broadcastChatMessage(const std::string &user, const std::string &message) {
	pthread_mutex_lock(&m_Mutex);

	for (std::map<std::string, User*>::iterator it=m_UserMap.begin(); it!=m_UserMap.end(); ++it) {
		User *other=(*it).second;
		other->getProtocol()->sendChatMessage(user, message);
	}

	pthread_mutex_unlock(&m_Mutex);
}
