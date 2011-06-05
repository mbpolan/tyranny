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
// mainwindow.cpp: implementation of the MainWindow class

#include <QMessageBox>

#include "authdialog.h"
#include "iohandler.h"
#include "mainwindow.h"
#include "prefdialog.h"
#include "statsdialog.h"

#include "ui/ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
	ui=new Ui::MainWindow;
	ui->setupUi(this);

	// connect signals
	connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(onConnect()));
	connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(onDisconnect()));
	connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(onPreferences()));
	connect(ui->actionStatistics, SIGNAL(triggered()), this, SLOT(onStatistics()));
	connect(ui->sendButton, SIGNAL(clicked()), this, SLOT(onSendButtonClicked()));

	// try to load the preferences file
	QString ip;
	int port;
	QVector<QPair<QString, int> > servers;

	IOHandler io("client.dat");
	if (io.loadPreferences(&ip, &port, &servers))
	    m_PrefData=new PrefDialog::Data(ip, port, servers);
	else
	    m_PrefData=NULL;

	// set some defaults
	m_Network=NULL;

	// toggle the interface initially
	toggleUi(false);
}

MainWindow::~MainWindow() {
	if (m_PrefData)
		delete m_PrefData;
}

void MainWindow::onConnect() {
	// do not even try to connect if no preferences data is given
	if (!m_PrefData)
		return;

	// allocate a new network manager if needed
	if (m_Network)
		delete m_Network;

	m_Network=new NetManager(this);

	// connect signals
	connect(m_Network, SIGNAL(connected()), this, SLOT(onNetConnected()));
	connect(m_Network, SIGNAL(disconnected()), this, SLOT(onNetDisconnected()));
	connect(m_Network, SIGNAL(networkError(QString)), this, SLOT(onNetError(QString)));
	connect(m_Network, SIGNAL(criticalError(QString)), this, SLOT(onNetCriticalError(QString)));
	connect(m_Network, SIGNAL(networkMessage(QString)), this, SLOT(onNetMessage(QString)));
	connect(m_Network, SIGNAL(requireAuthentication()), this, SLOT(onNetAuthenticate()));
	connect(m_Network, SIGNAL(userLoggedIn(QString)), this, SLOT(onNetUserLoggedIn(QString)));
	connect(m_Network, SIGNAL(userLoggedOut(QString)), this, SLOT(onNetUserLoggedOut(QString)));
	connect(m_Network, SIGNAL(lobbyChatMessage(QString,QString)), this, SLOT(onNetLobbyChatMessage(QString,QString)));
	connect(m_Network, SIGNAL(userStatistics(int,int,int,int)), this, SLOT(onNetStatistics(int,int,int,int)));

	m_Network->connectToServer(m_PrefData->getIP(), m_PrefData->getPort());
}

void MainWindow::onDisconnect() {
	m_Network->disconnectFromServer();
}

void MainWindow::onPreferences() {
	PrefDialog pd(m_PrefData, this);
	if (pd.exec()) {
		// we don't need the old data anymore
		if (m_PrefData)
		    delete m_PrefData;

		// update preferences data
		m_PrefData=pd.getPreferencesData();

		// save the new preferences data to file
		IOHandler io("client.dat");
		io.savePreferences(m_PrefData->getIP(), m_PrefData->getPort(), m_PrefData->getServers());
	}
}

void MainWindow::onQuit() {

}

void MainWindow::onStatistics() {
	// request the user's statistics
	m_Network->requestStatistics();
}

void MainWindow::onSendButtonClicked() {
	// check to see if the user entered any message
	if (!ui->chatEdit->text().isEmpty())
		m_Network->sendChatMessage(ui->chatEdit->text());
}

void MainWindow::onNetConnected() {
	ui->statusbar->showMessage(tr("Connected."));

	// toggle status of the interface
	toggleUi(true);
}

void MainWindow::onNetDisconnected() {
	ui->statusbar->showMessage(tr("Disconnected."));

	// clean up the interface elements
	ui->userList->clear();
	ui->roomList->clear();
	ui->chatBox->clear();

	// toggle status of the interface
	toggleUi(false);
}

void MainWindow::onNetAuthenticate() {
	// present the user with an authentication dialog
	AuthDialog ad(this);
	if (ad.exec()==QDialog::Accepted) {
		// grab the inputted values
		QString username=ad.getUsername();
		QString password=ad.getPassword();

		// send the credentials
		m_Network->sendAuthentication(username, password);
	}

	else
		m_Network->disconnectFromServer();
}

void MainWindow::onNetError(const QString &error) {
	ui->statusbar->showMessage(error);
}

void MainWindow::onNetCriticalError(const QString &error) {
    QMessageBox::critical(this, tr("Error"), error, QMessageBox::Ok, QMessageBox::NoButton);
}

void MainWindow::onNetMessage(const QString &msg) {
	ui->statusbar->showMessage(msg);
}

void MainWindow::onNetUserLoggedIn(const QString &username) {
	// create a new item for this user
	QTreeWidgetItem *item=new QTreeWidgetItem(ui->userList);
	item->setText(0, username);
	item->setText(1, "0");
}

void MainWindow::onNetUserLoggedOut(const QString &username) {
	// start by finding the item with the given username text in the user list
	for (int i=0; i<ui->userList->topLevelItemCount(); i++) {
		QTreeWidgetItem *item=ui->userList->topLevelItem(i);
		if (item->text(0)==username) {
			ui->userList->takeTopLevelItem(i);
			delete item;
		}
	}
}

void MainWindow::onNetLobbyChatMessage(const QString &user, const QString &message) {
	// format the string
	QString line="<b>"+user+"</b>";
	line+=": ";
	line+=message;
	line+="<br>\n";

	// append the message to the chat buffer
	ui->chatBox->insertHtml(line);
}

void MainWindow::onNetStatistics(int points, int gamesPlayed, int won, int lost) {
	// show the statistics dialog
	StatsDialog sd(points, gamesPlayed, won, lost, this);
	sd.exec();
}

void MainWindow::toggleUi(bool connected) {
	// toggle menu actions
	ui->actionBlocked_Users->setEnabled(connected);
	ui->actionConnect->setEnabled(!connected);
	ui->actionDisconnect->setEnabled(connected);
	ui->actionCreate_Room->setEnabled(connected);
	ui->actionEdit_Profile->setEnabled(connected);
	ui->actionManage_Friends->setEnabled(connected);
	ui->actionSettings->setEnabled(connected);
	ui->actionStatistics->setEnabled(connected);

	// toggle the widgets
	ui->chatEdit->setEnabled(connected);
	ui->sendButton->setEnabled(connected);
}
