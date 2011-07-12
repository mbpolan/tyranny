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
// gamewindow.cpp: implementation of the GameWindow class.

#include <QMessageBox>

#include "boardview.h"
#include "gamewindow.h"

#include "ui/ui_gamewindow.h"

GameWindow::GameWindow(int gid, const QString &username, const QString &host, int port, QWidget *parent): QMainWindow(parent) {
	ui=new Ui::GameWindow;
	ui->setupUi(this);

	// replace the separator placeholder widget with the board view
	BoardView *view=new BoardView(this);
	ui->glLayout->addWidget(view);

	// create the network handler
	m_Network=new GameProtocol(gid, username, this);

	// connect signals
	connect(m_Network, SIGNAL(connected()), this, SLOT(onNetConnected()));
	connect(m_Network, SIGNAL(disconnected()), this, SLOT(onNetDisconnected()));
	connect(m_Network, SIGNAL(networkError(QString)), this, SLOT(onNetError(QString)));
	connect(m_Network, SIGNAL(startWait()), this, SLOT(onNetStartWait()));

	// attempt a connection to the game server
	m_Network->connectToServer(host, port);
}

void GameWindow::onNetConnected() {
	statusBar()->showMessage(tr("Connected to server. Authorizing..."));
}

void GameWindow::onNetDisconnected() {
	statusBar()->showMessage(tr("Disconnected from game server. Closing interface..."));
}

void GameWindow::onNetError(const QString &message) {
	statusBar()->showMessage(QString("*** ")+message);
}

void GameWindow::onNetStartWait() {
	QMessageBox::question(this, tr("Waiting"), tr("The game is ready to begin. "
								    "You can either wait for more players or start now."), QMessageBox::Ok);

	// once the dialog is dismissed, send the game server a reply
	m_Network->beginGame();
}
