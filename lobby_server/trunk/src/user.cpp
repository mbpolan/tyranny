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
// user.cpp: implementation of the User class.

#include "user.h"

User::User(const std::string &username, const std::string &password) {
	m_Username=username;
	m_Password=password;
	m_Protocol=NULL;
}

bool User::isFriendsWith(const std::string &username) const {
	for (int i=0; i<m_Friends.size(); i++) {
		if (m_Friends[i]==username)
			return true;
	}

	return false;
}

bool User::isBlocking(const std::string &username) const {
	for (int i=0; i<m_Blocked.size(); i++) {
		if (m_Blocked[i]==username)
			return true;
	}

	return false;
}
