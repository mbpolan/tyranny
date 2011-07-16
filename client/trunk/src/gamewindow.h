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
// gamewindow.h: definition of the GameWindow class.

#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

#include "gameprotocol.h"
#include "tokenchooser.h"

namespace Ui {
	class GameWindow;
}

class GameWindow: public QMainWindow {
	Q_OBJECT

	public:
		/**
		 * Creates a blank room window for the given username.
		 *
		 * @param gid The room's id number.
		 * @param username The user who is logged in.
		 * @param host The host game server to connect to.
		 * @param port The host game server's port.
		 * @param parent The parent for this window.
		 */
		GameWindow(int gid, const QString &username, const QString &host, int port, QWidget *parent=NULL);

	private slots:
		/// Handler for telling the game server to start the game.
		void onBeginGame();

		/// Network handler for successful connections.
		void onNetConnected();

		/// Network handler for disconnect.
		void onNetDisconnected();

		/// Network handler for critical network errors.
		void onNetError(const QString &message);

		/// Network handler for start/wait dialog.
		void onNetStartWait();

		/// Network handler for turn order data.
		void onNetTurnOrder(const QVector<int> &order);

		/// Network handler for players joining.
		void onNetPlayerJoined(const QString &username, int index);

		/// Network handler for players quitting.
		void onNetPlayerQuit(int index);

		/// Network handler for showing the token selection dialog.
		void onNetTokenSelection();

	private:
		Ui::GameWindow *ui;

		/// List of players.
		QVector<QString> m_Players;

		/// Dialog to prompt room owners to begin the game.
		QMessageBox *m_BeginGameMsgBox;

		/// Token chooser dialog.
		TokenChooser *m_TokChooser;

		/// Network protocol handler.
		GameProtocol *m_Network;
};

#endif
