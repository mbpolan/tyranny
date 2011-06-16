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

#include "gameprotocol.h"

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
		/// Network handler for successful connections.
		void onNetConnected();

		/// Network handler for disconnect.
		void onNetDisconnected();

		/// Network handler for critical network errors.
		void onNetError(const QString &message);

	private:
		Ui::GameWindow *ui;

		/// Network protocol handler.
		GameProtocol *m_Network;
};

#endif
