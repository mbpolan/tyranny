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
// player.h: definition of the Player class.

#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>

class Player {
	public:
		/**
		 * Creates a player model object.
		 *
		 * @param username The player's username.
		 */
		Player(const std::string &username);

		/**
		 * Returns the player's username.
		 *
		 * @return The player's username.
		 */
		std::string getUsername() const { return m_Username; }

	private:
		/// The player's username.
		std::string m_Username;
};

#endif
