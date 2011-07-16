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
#include "turndialog.h"

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
	connect(m_Network, SIGNAL(playerJoined(QString,int)), this, SLOT(onNetPlayerJoined(QString,int)));
	connect(m_Network, SIGNAL(playerQuit(int)), this, SLOT(onNetPlayerQuit(int)));
	connect(m_Network, SIGNAL(turnOrder(QVector<int>)), this, SLOT(onNetTurnOrder(QVector<int>)));
	connect(m_Network, SIGNAL(tokenSelection()), this, SLOT(onNetTokenSelection()));

	// create supplemental dialogs
	m_BeginGameMsgBox=new QMessageBox(QMessageBox::Information, tr("Waiting..."),
						    tr("The game room is ready to begin. You may choose to wait for more players, or"
							 "start the game beforehand. Click OK to start the game."), QMessageBox::Ok, this);
	m_BeginGameMsgBox->hide();
	connect(m_BeginGameMsgBox, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onBeginGame()));

	// attempt a connection to the game server
	m_Network->connectToServer(host, port);

	// prepare other variables
	m_Players=QVector<QString>(4);
	m_TokChooser=NULL;
}

void GameWindow::onBeginGame() {
	m_BeginGameMsgBox->hide();
	m_Network->beginGame();
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
	m_BeginGameMsgBox->show();
}

void GameWindow::onNetPlayerJoined(const QString &username, int index) {
	QTreeWidgetItem *item=new QTreeWidgetItem(ui->userList);
	item->setText(0, username);

	m_Players[index]=username;
}

void GameWindow::onNetPlayerQuit(int index) {
	// remove the player in question
	for (int i=0; i<ui->userList->topLevelItemCount(); i++) {
		QTreeWidgetItem *item=ui->userList->topLevelItem(i);
		if (item->text(0)==m_Players[index])
			delete ui->userList->takeTopLevelItem(i);
	}

	m_Players[index]="";
}

void GameWindow::onNetTurnOrder(const QVector<int> &order) {
	TurnDialog td(m_Players, order, this);
	td.animate();
	td.exec();
}

void GameWindow::onNetTokenSelection() {
	// allocate the token chooser dialog and show it
	m_TokChooser=new TokenChooser(m_Players, this);
	m_TokChooser->show();
}
