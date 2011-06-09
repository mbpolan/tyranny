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
// roomdata.h: definition and implementation of the RoomData class.

#ifndef ROOMDATA_H
#define ROOMDATA_H

#include <QObject>

/**
 * A model representing data about a game room.
 */
class RoomData {
	public:
		enum Status { Open, InProgress, Closed };
		enum Type { Public, Private };

	public:
		/// Creates an empty model.
		RoomData() { }

		/**
		 * Creates a model of a room with the given parameters.
		 *
		 * @param gid The room's id number.
		 * @param owner The room's owner.
		 * @param playerCount The number of players in the room.
		 * @param status The room status.
		 * @param type The type of room.
		 */
		RoomData(int gid, const QString &owner, int playerCount, const Status &status, const Type &type) {
			m_Gid=gid;
			m_Owner=owner;
			m_PlayerCount=playerCount;
			m_Status=status;
			m_Type=type;
		}

		/**
		 * Returns the room's id number.
		 *
		 * @return The id number of the room.
		 */
		int getGid() const { return m_Gid; }

		/**
		 * Returns the room's owner.
		 *
		 * @return Username of the owner.
		 */
		QString getOwner() const { return m_Owner; }

		/**
		 * Returns the amount of players in the room.
		 *
		 * @return The current player count.
		 */
		int getPlayerCount() const { return m_PlayerCount; }

		/**
		 * Returns the room status.
		 *
		 * @return The current status of the room.
		 */
		Status getStatus() const { return m_Status; }

		/**
		 * Returns the room type.
		 *
		 * @return The type of room this is.
		 */
		Type getType() const { return m_Type; }

	private:
		/// The id number of the room.
		int m_Gid;

		/// The username of the owner.
		QString m_Owner;

		/// The amount of players.
		int m_PlayerCount;

		/// The status of the room.
		Status m_Status;

		/// The type of room.
		Type m_Type;
};

#endif
