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
#include "roomdata.h"

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
		enum UserRequest { FriendRequest, BlockRequest };

		enum UserStatus { UserNone, UserFriend, UserBlocked };

		enum RedistMethod { RandomToPlayers, ReturnToBank };

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

		/**
		 * Sends a chat message to the lobby.
		 *
		 * @param message The chat message to send.
		 */
		void sendChatMessage(const QString &message);

		/**
		 * Sends a request to the server for this user's friend list.
		 */
		void requestFriendList();

		/**
		 * Sends the server an updated list of friends.
		 *
		 * @param list The new friend list.
		 */
		void sendFriendListUpdate(const QStringList &list);

		/**
		 * Sends a request to the server for this user's blocked user list.
		 */
		void requestBlockedList();

		/**
		 * Sends the server an updated list of blocked users.
		 *
		 * @param list The new blocked user list.
		 */
		void sendBlockedListUpdate(const QStringList &list);

		/**
		 * Sends a request to the server for this user's statistics.
		 */
		void requestStatistics();

		/**
		 * Sends a request to the server for this user's profile.
		 */
		void requestUserProfile();

		/**
		 * Sends the server an update user profile.
		 *
		 * @param name The user's real name.
		 * @param email The user's email address.
		 * @param age The user's age.
		 * @param bio The user's biography.
		 */
		void sendUserProfileUpdate(const QString &name, const QString &email, int age, const QString &bio);

		/**
		 * Sends the server a new password for this user.
		 *
		 * @param password The new password.
		 */
		void sendPasswordChange(const QString &password);

		/**
		 * Sends the server a request to add a user to either the friends list or blocked user list.
		 *
		 * @param username The username to add.
		 * @param list The list to add the user to.
		 */
		void sendUserRequest(const QString &username, const UserRequest &list);

		/**
		 * Sends a request to the server to create a new game room with the given parameters.
		 *
		 * @param maxTurns Maximum number of game turns.
		 * @param maxHumans Maximum number of human players.
		 * @param freeParkReward Money paid to the player for stepping on Free Parking.
		 * @param propMethod Property redistribution method following player bankruptcy.
		 * @param incomeTaxChoice Whether or not players have a choice of free when stepping on Income Tax.
		 * @param password Room password.
		 * @param onlyFriends Whether or not only friends can join this room.
		 */
		void sendCreateRoom(int maxTurns, int maxHumans, int freeParkReward, const RedistMethod &propMethod,
					  bool incomeTaxChoice, const QString &password, bool onlyFriends);

		/**
		 * Sends a request to the server to join the game room with the given id number.
		 *
		 * @param gid The room's id number.
		 * @param password The room's password, or empty string if none.
		 */
		void sendJoinRoom(int gid, const QString &password);

		/**
		 * Sends a request to the server to refresh the list of rooms.
		 */
		void sendRoomListRefresh();

	signals:
		/// Signal emitted when a connection is established.
		void connected();

		/// Signal emitted when authentication is necessary.
		void requireAuthentication();

		/// Signal emitted when a connection is terminated.
		void disconnected();

		/// Signal emitted when a general network error occurs.
		void networkError(const QString &msg);

		/// Signal emitted when a general message should be displayed in the status bar.
		void statusMessage(const QString &msg);

		/// Signal emitted when another user logs into the lobby.
		void userLoggedIn(const QString &username, const NetManager::UserStatus &status);

		/// Signal emitted when another user logs out of the lobby.
		void userLoggedOut(const QString &username);

		/// Signal emitted when a chat message is sent in the lobby.
		void lobbyChatMessage(const QString &sender, const QString &message);

		/// Signal emitted when the server has responded with the user's friends list.
		void userFriendList(const QStringList &friends);

		/// Signal emitted when the server has responded with the user's blocked list.
		void userBlockedList(const QStringList &blocked);

		/// Signal emitted when the server has responded with the user's statistics.
		void userStatistics(int points, int gamesPlayed, int won, int lost);

		/// Signal emitted when the server has responded with the user's profile data.
		void userProfile(const QString &name, const QString &email, int age, const QString &bio);

		/// Signal emitted when the server has sent an informational message.
		void serverInfo(const QString &message);

		/// Signal emitted when the server has sent an error message.
		void serverError(const QString &message);

		/// Signal emitted when the user should connect to a game server.
		void joinGameServer(const QString &host, int port);

		/// Signal emitted when updated data about a room is available.
		void roomListUpdate(const RoomData &data);

		/// Signal emitted when a list of rooms is available.
		void roomListRefresh(const QVector<RoomData> &list);

	private slots:
		/// Handler for socket error condition.
		void onError(QAbstractSocket::SocketError error);

		/// Handler for data read.
		void onReadData();

	private:
		/**
		 * Parses and evaluates incoming packets.
		 * @param p The packet to parse.
		 */
		void parsePacket(Packet &p);

		/**
		 * Parses a packet containing data about a user who logged in.
		 * @param p The packet to parse.
		 */
		void handleUserLoggedIn(Packet &p);

		/**
		 * Parses a sent chat message to the lobby.
		 * @param p The packet to parse.
		 */
		void handleLobbyChatMessage(Packet &p);

		/**
		 * Parses a packet containing user statistics.
		 * @param p The packet to parse.
		 */
		void handleStatistics(Packet &p);

		/**
		 * Parses a packet containing a user's profile data.
		 * @param p The packet to parse.
		 */
		void handleUserProfileRequest(Packet &p);

		/**
		 * Parses a packet containing the user's friend list.
		 * @param p The packet to parse.
		 */
		void handleFriendListRequest(Packet &p);

		/**
		 * Parses a packet containing the user's blocked list.
		 * @param p The packet to parse.
		 */
		void handleBlockedListRequest(Packet &p);

		/**
		 * Parses a packet containing the response for creating a room.
		 * @param p The packet to parse.
		 */
		void handleCreateRoomResponse(Packet &p);

		/**
		 * Parses a packet containing the response for joining a room.
		 * @param p The packet to parse.
		 */
		void handleJoinRoomResponse(Packet &p);

		/**
		 * Parses a packet containing updated data about a room.
		 * @param p The packet to parse.
		 */
		void handleRoomListUpdate(Packet &p);

		/**
		 * Parses a packet containing a list of rooms.
		 * @param p The packet to parse.
		 */
		void handleRoomListRefresh(Packet &p);

		/// Communications socket.
		QTcpSocket *m_Socket;
};

#endif
