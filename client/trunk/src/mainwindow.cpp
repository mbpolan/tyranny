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

#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>

#include "authdialog.h"
#include "iohandler.h"
#include "mainwindow.h"
#include "prefdialog.h"
#include "profiledialog.h"
#include "settingsdialog.h"
#include "statsdialog.h"
#include "userlistdialog.h"

#include "ui/ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
	ui=new Ui::MainWindow;
	ui->setupUi(this);

	// connect signals
	connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(onConnect()));
	connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(onDisconnect()));
	connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(onPreferences()));
	connect(ui->actionManage_Friends, SIGNAL(triggered()), this, SLOT(onManageFriends()));
	connect(ui->actionBlocked_Users, SIGNAL(triggered()), this, SLOT(onBlockedUsers()));
	connect(ui->actionEdit_Profile, SIGNAL(triggered()), this, SLOT(onEditProfile()));
	connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(onSettings()));
	connect(ui->actionStatistics, SIGNAL(triggered()), this, SLOT(onStatistics()));
	connect(ui->sendButton, SIGNAL(clicked()), this, SLOT(onSendButtonClicked()));
	connect(ui->userList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onUserListContextMenu(QPoint)));

	// set policies
	ui->userList->setContextMenuPolicy(Qt::CustomContextMenu);

	// install event filters
	ui->chatEdit->installEventFilter(this);

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
	connect(m_Network, SIGNAL(statusMessage(QString)), this, SLOT(onNetMessage(QString)));
	connect(m_Network, SIGNAL(requireAuthentication()), this, SLOT(onNetAuthenticate()));
	connect(m_Network, SIGNAL(userLoggedIn(QString)), this, SLOT(onNetUserLoggedIn(QString)));
	connect(m_Network, SIGNAL(userLoggedOut(QString)), this, SLOT(onNetUserLoggedOut(QString)));
	connect(m_Network, SIGNAL(lobbyChatMessage(QString,QString)), this, SLOT(onNetLobbyChatMessage(QString,QString)));
	connect(m_Network, SIGNAL(userProfile(QString,QString,int,QString)), this, SLOT(onNetUserProfile(QString,QString,int,QString)));
	connect(m_Network, SIGNAL(userStatistics(int,int,int,int)), this, SLOT(onNetStatistics(int,int,int,int)));
	connect(m_Network, SIGNAL(userFriendList(QStringList)), this, SLOT(onNetFriendList(QStringList)));
	connect(m_Network, SIGNAL(userBlockedList(QStringList)), this, SLOT(onNetBlockedList(QStringList)));
	connect(m_Network, SIGNAL(serverInfo(QString)), this, SLOT(onNetInfoMessage(QString)));
	connect(m_Network, SIGNAL(serverError(QString)), this, SLOT(onNetErrorMessage(QString)));

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
	qApp->quit();
}

void MainWindow::onManageFriends() {
	// request the user's friend list
	m_Network->requestFriendList();
}

void MainWindow::onBlockedUsers() {
	// request the user's blocked user list
	m_Network->requestBlockedList();
}

void MainWindow::onEditProfile() {
	// request the user's profile data
	m_Network->requestUserProfile();
}

void MainWindow::onStatistics() {
	// request the user's statistics
	m_Network->requestStatistics();
}

void MainWindow::onSettings() {
	// present the user with a settings editor dialog
	SettingsDialog sd(this);
	if (sd.exec()==QDialog::Accepted) {
		if (sd.didChangePassword())
			m_Network->sendPasswordChange(sd.getNewPassword());
	}
}

void MainWindow::onSendButtonClicked() {
	// check to see if the user entered any message
	if (!ui->chatEdit->text().isEmpty()) {
		m_Network->sendChatMessage(ui->chatEdit->text());

		// also clear the line edit
		ui->chatEdit->clear();
	}
}

void MainWindow::onUserListContextMenu(const QPoint &pos) {
	// create some actions and toggle them
	QAction *friendAct=new QAction(tr("Add as Friend"), this);
	QAction *blockAct=new QAction(tr("Block User"), this);

	// see if a user is selected
	QTreeWidgetItem *item=ui->userList->currentItem();
	friendAct->setEnabled((item!=NULL));
	blockAct->setEnabled((item!=NULL));

	QMenu context(tr("Context Menu"), this);
	context.addAction(friendAct);
	context.addAction(blockAct);

	QAction *result;
	if ((result=context.exec(mapToGlobal(pos))) && item) {
		// get the target username
		QString username=item->text(0);

		// and request the action
		if (result==friendAct)
			m_Network->sendUserRequest(username, NetManager::FriendRequest);
		else
			m_Network->sendUserRequest(username, NetManager::BlockRequest);
	}
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
	ui->statusbar->showMessage(QString("*** ")+error);
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

void MainWindow::onNetUserProfile(const QString &name, const QString &email, int age, const QString &bio) {
	// show the profile dialog
	ProfileDialog pd(name, email, age, bio, this);
	if (pd.exec()==QDialog::Accepted)
		m_Network->sendUserProfileUpdate(pd.getName(), pd.getEmailAddress(), pd.getAge(), pd.getBiography());
}

void MainWindow::onNetStatistics(int points, int gamesPlayed, int won, int lost) {
	// show the statistics dialog
	StatsDialog sd(points, gamesPlayed, won, lost, this);
	sd.exec();
}

void MainWindow::onNetFriendList(const QStringList &list) {
	UserListDialog ld("Friends", list, this);
	if (ld.exec()==QDialog::Accepted)
		m_Network->sendFriendListUpdate(ld.getUserList());
}

void MainWindow::onNetBlockedList(const QStringList &list) {
	UserListDialog ld("Blocked Users", list, this);
	if (ld.exec()==QDialog::Accepted)
		m_Network->sendBlockedListUpdate(ld.getUserList());
}

void MainWindow::onNetInfoMessage(const QString &msg) {
	QMessageBox::information(this, tr("Information"), msg, QMessageBox::Ok, QMessageBox::NoButton);
}

void MainWindow::onNetErrorMessage(const QString &msg) {
	QMessageBox::critical(this, tr("Error"), msg, QMessageBox::Ok, QMessageBox::NoButton);
}

bool MainWindow::eventFilter(QObject *sender, QEvent *e) {
	if (sender==ui->chatEdit && e->type()==QEvent::KeyPress) {
		QKeyEvent *ke=static_cast<QKeyEvent*>(e);
		if (ke->key()==Qt::Key_Return)
			onSendButtonClicked();
	}

	return QMainWindow::eventFilter(sender, e);
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
