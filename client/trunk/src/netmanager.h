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
// netmanager.h: definition of the NetManager class.

#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <QObject>
#include <QTcpSocket>

#include "packet.h"

/**
 * Manager for most network operations required by the client.
 * Instead of dealing with low-level socket functions, this manager class
 * wraps that functionality in a Qt-compliant way. The class emits signals based on
 * network status and connectivity, and also provides functions that generalize authentication,
 * connecting to the server, and disconnecting from the server.
 */
class NetManager: public QObject {
	Q_OBJECT

	public:
		/**
		 * Default constructor.
		 */
		NetManager(QObject *parent=NULL);

		/**
		 * Attempts to connect to the given server.
		 *
		 * @param server The server name or IP address.
		 * @param port The port number.
		 */
		void connectToServer(const QString &server, int port);

		/**
		 * Disconnects from the current server.
		 */
		void disconnectFromServer();

		/**
		 * Sends a username and password for authentication to the server.
		 *
		 * @param username The username.
		 * @param password The password.
		 */
		void sendAuthentication(const QString &username, const QString &password);

	signals:
		/// Signal emitted when a connection is established.
		void connected();

		/// Signal emitted when authentication is necessary.
		void requireAuthentication();

		/// Signal emitted when a connection is terminated.
		void disconnected();

		/// Signal emitted when a general network error occurs.
		void networkError(const QString &msg);

		/// Signal emitted when a critical non-network error occurs.
		void criticalError(const QString &msg);

		/// Signal emitted when a general message should be displayed in the status bar.
		void networkMessage(const QString &msg);

	private slots:
		/// Handler for socket error condition.
		void onError(QAbstractSocket::SocketError error);

		/// Handler for data read.
		void onReadData();

	private:
		/**
		 * Parses and evaluates incoming packets.
		 *
		 * @param p The packet to parse.
		 */
		void parsePacket(Packet &p);

		/// Communications socket.
		QTcpSocket *m_Socket;
};

#endif
