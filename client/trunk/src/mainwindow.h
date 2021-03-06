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
// mainwindow.h: declaration of the MainWindow class

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include "gamewindow.h"
#include "prefdialog.h"
#include "netmanager.h"

namespace Ui {
	class MainWindow;
}

/**
 * A class that serves as the controller for the client's main window.
 * This class abstracts the interface for the window that provides the user with
 * a way to interact with the client when he or she is in the lobby. It is responsible
 * for handling all of the related menu actions, widget events, and network communications
 * which require data to be presented to the user. Here is where the list of open rooms is
 * displayed and managed, and where the general chat box resides.
 *
 * The UI file for this class is named mainwindow.ui.
 */
class MainWindow: public QMainWindow {
	Q_OBJECT

	public:
		/**
		 * Default constructor for the MainWindow class.
		 * The parent parameter should always be left as NULL since it is illogical for
		 * this window to have a parent.
		 */
		MainWindow(QWidget *parent=NULL);

		/**
		 * Default destructor for the MainWindow class.
		 */
		~MainWindow();

	private slots:
		/// Handler for File -> Connect action.
		void onConnect();

		/// Handler for File -> Disconnect action.
		void onDisconnect();

		/// Handler for File -> Preferences action.
		void onPreferences();

		/// Handler for File -> Quit action.
		void onQuit();

		/// Handler for Game -> Create Room
		void onCreateRoom();

		/// Handler for Game -> Manage Friends action.
		void onManageFriends();

		/// Handler for Game -> Blocked Users action.
		void onBlockedUsers();

		/// Handler for Account -> Edit Profile action.
		void onEditProfile();

		/// Handler for Account -> Statistics action.
		void onStatistics();

		/// Handler for Account -> Settings action.
		void onSettings();

		/// Handler for "Send" button clicks
		void onSendButtonClicked();

		/// Handler for context menu requests on the user list.
		void onUserListContextMenu(const QPoint &pos);

		/// Handler for context menu requests on the user list.
		void onRoomListContextMenu(const QPoint &pos);

		/// Network handler for an established connection.
		void onNetConnected();

		/// Network handler for disconnected state.
		void onNetDisconnected();

		/// Network handler for authentication request.
		void onNetAuthenticate();

		/// Network handler for socket errors.
		void onNetError(const QString &error);

		/// Network handler for general messages.
		void onNetMessage(const QString &msg);

		/// Network handler for when another user logs in.
		void onNetUserLoggedIn(const QString &username, const NetManager::UserStatus &status);

		/// Network handler for when another user logs out.
		void onNetUserLoggedOut(const QString &username);

		/// Network handler for chat messages sent from other users in the lobby.
		void onNetLobbyChatMessage(const QString &user, const QString &message);

		/// Network handler for statistics information.
		void onNetStatistics(int points, int gamesPlayed, int won, int lost);

		/// Network handler for user profile information.
		void onNetUserProfile(const QString &name, const QString &email, int age, const QString &bio);

		/// Network handler for friend list response.
		void onNetFriendList(const QStringList &list);

		/// Network handler for blocked list response.
		void onNetBlockedList(const QStringList &list);

		/// Network handler for informational server messages.
		void onNetInfoMessage(const QString &msg);

		/// Network handler for error server messages.
		void onNetErrorMessage(const QString &msg);

		/// Network handler for joining a game server.
		void onNetJoinGameServer(int gid, const QString &host, int port);

		/// Network handler for updating a room.
		void onNetRoomListUpdate(const RoomData &room);

		/// Network handler for removing old rooms.
		void onNetRoomListDelete(int gid);

		/// Network handler for updating the entire list of rooms.
		void onNetRoomListRefresh(const QVector<RoomData> &list);

	protected:
		/// Event handler for chat QLineEdit key presses
		bool eventFilter(QObject *sender, QEvent *e);

	private:
		/**
		 * Prepares a user to join a game room.
		 */
		void prepareJoinRoom();

		/**
		 * Enables or disables parts of the interface based on connectivity.
		 *
		 * @param connected True if the client is connected and authenticated, false otherwise.
		 */
		void toggleUi(bool connected);

		/// Pointer to the user interface object for this class.
		Ui::MainWindow *ui;

		/// Cached preferences data.
		PrefDialog::Data *m_PrefData;

		/// Network manager object.
		NetManager *m_Network;

		/// The current room's game window.
		GameWindow *m_GameWnd;

		/// The username of the currently logged in user.
		QString m_LoggedInUser;
};

#endif
