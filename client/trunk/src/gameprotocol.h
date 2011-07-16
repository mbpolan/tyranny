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
// gameprotocol.h: definition of the GameProtocol class.

#ifndef GAMEPROTOCOL_H
#define GAMEPROTOCOL_H

#include <QObject>
#include <QTcpSocket>

#include "packet.h"

class GameProtocol: public QObject {
	Q_OBJECT

	public:
		/**
		 * Creates a new game protocol manager.
		 *
		 * @param gid The room id we are attempting to join.
		 * @param username The username of the currently logged in user.
		 */
		GameProtocol(int gid, const QString &username, QObject *parent=NULL);

		/// Destructor.
		virtual ~GameProtocol();

		/**
		 * Connects to the game server given by the connection information.
		 *
		 * @param host The hostname/IP address of the game server.
		 * @param port The port of the game server.
		 */
		void connectToServer(const QString &host, int port);

		/**
		 * Disconnects from the current game server.
		 */
		void disconnectFromServer();

		/**
		 * Tells the game server to begin the room's game.
		 */
		void beginGame();

	public slots:
		/// Handler for establishing a connection to server.
		void onConnected();

		/// Handler for disconnecting from a server.
		void onDisconnected();

		/// Handler for socket errors.
		void onError(const QAbstractSocket::SocketError &error);

		/// Handler for reading data from the socket
		void onDataReady();

	signals:
		/// Signal emitted when a connection is established to the game server.
		void connected();

		/// Signal emitted when the current connection is terminated.
		void disconnected();

		/// Signal emitted when a message from the server arrives.
		void networkError(const QString &message);

		/// Signal emitted when the room is waiting to begin.
		void startWait();

		/// Signal emitted when turn orders are available.
		void turnOrder(const QVector<int> &order);

		/// Signal emitted when a player joins the room.
		void playerJoined(const QString &username, int index);

		/// Signal emitted when a player quits the room.
		void playerQuit(int index);

	private:
		/// Handles parsing an incoming packet.
		void parsePacket(Packet &p);

		/// Handles parsing a packet containing data about a player who joined the room.
		void handlePlayerJoined(Packet &p);

		/// Handles parsing a packet containing turn order data.
		void handleTurnOrder(Packet &p);

		/// The socket this protocol communicates with.
		QTcpSocket *m_Socket;

		/// The room id number to join.
		int m_Gid;

		/// The username of the logged in user.
		QString m_Username;
};

#endif
