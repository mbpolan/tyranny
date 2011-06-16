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
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <sstream>

#include "authdialog.h"
#include "gamewindow.h"
#include "iohandler.h"
#include "mainwindow.h"
#include "prefdialog.h"
#include "profiledialog.h"
#include "rulesdialog.h"
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
	connect(ui->actionCreate_Room, SIGNAL(triggered()), this, SLOT(onCreateRoom()));
	connect(ui->actionManage_Friends, SIGNAL(triggered()), this, SLOT(onManageFriends()));
	connect(ui->actionBlocked_Users, SIGNAL(triggered()), this, SLOT(onBlockedUsers()));
	connect(ui->actionEdit_Profile, SIGNAL(triggered()), this, SLOT(onEditProfile()));
	connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(onSettings()));
	connect(ui->actionStatistics, SIGNAL(triggered()), this, SLOT(onStatistics()));
	connect(ui->sendButton, SIGNAL(clicked()), this, SLOT(onSendButtonClicked()));
	connect(ui->userList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onUserListContextMenu(QPoint)));
	connect(ui->roomList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onRoomListContextMenu(QPoint)));

	// set policies
	ui->userList->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->roomList->setContextMenuPolicy(Qt::CustomContextMenu);

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
	m_LoggedInUser="";

	// toggle the interface initially
	toggleUi(false);
}

MainWindow::~MainWindow() {
	if (m_PrefData)
		delete m_PrefData;
}

void MainWindow::onConnect() {
	// do not even try to connect if no preferences data is given
	if (!m_PrefData) {
		QMessageBox::warning(this, tr("Warning"), tr("You need to set connection details in File -> Preferences first."));
		return;
	}

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
	connect(m_Network, SIGNAL(userLoggedIn(QString,NetManager::UserStatus)), this, SLOT(onNetUserLoggedIn(QString,NetManager::UserStatus)));
	connect(m_Network, SIGNAL(userLoggedOut(QString)), this, SLOT(onNetUserLoggedOut(QString)));
	connect(m_Network, SIGNAL(lobbyChatMessage(QString,QString)), this, SLOT(onNetLobbyChatMessage(QString,QString)));
	connect(m_Network, SIGNAL(userProfile(QString,QString,int,QString)), this, SLOT(onNetUserProfile(QString,QString,int,QString)));
	connect(m_Network, SIGNAL(userStatistics(int,int,int,int)), this, SLOT(onNetStatistics(int,int,int,int)));
	connect(m_Network, SIGNAL(userFriendList(QStringList)), this, SLOT(onNetFriendList(QStringList)));
	connect(m_Network, SIGNAL(userBlockedList(QStringList)), this, SLOT(onNetBlockedList(QStringList)));
	connect(m_Network, SIGNAL(serverInfo(QString)), this, SLOT(onNetInfoMessage(QString)));
	connect(m_Network, SIGNAL(serverError(QString)), this, SLOT(onNetErrorMessage(QString)));
	connect(m_Network, SIGNAL(joinGameServer(int,QString,int)), this, SLOT(onNetJoinGameServer(int,QString,int)));
	connect(m_Network, SIGNAL(roomListUpdate(RoomData)), this, SLOT(onNetRoomListUpdate(RoomData)));
	connect(m_Network, SIGNAL(roomListDelete(int)), this, SLOT(onNetRoomListDelete(int)));
	connect(m_Network, SIGNAL(roomListRefresh(QVector<RoomData>)), this, SLOT(onNetRoomListRefresh(QVector<RoomData>)));

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

void MainWindow::onCreateRoom() {
	// present the user with the settings dialog
	RulesDialog rd(this);
	if (rd.exec()==QDialog::Accepted) {
		// get the data and send it to the server
		RulesDialog::Settings st=rd.getDefinedSettings();

		// find out the property redistribution method
		RulesDialog::Settings::PropertyRedistribution method=st.getPropertyRedistributionMethod();
		NetManager::RedistMethod prop=(method==RulesDialog::Settings::Random ? NetManager::RandomToPlayers : NetManager::ReturnToBank);


		// send this data to the server
		m_Network->sendCreateRoom(st.getMaxTurns(), st.getMaxHumans(), st.getFreeParkingReward(),
						  prop, st.getIncomeTaxChoice(), st.getRoomPassword(), st.getOnlyFriends());
	}
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
	if ((result=context.exec(ui->userList->mapToGlobal(pos))) && item) {
		// get the target username
		QString username=item->text(0);

		// request the action and style the list item
		if (result==friendAct) {
			m_Network->sendUserRequest(username, NetManager::FriendRequest);

			QBrush b;
			b.setColor(Qt::darkGreen);
			item->setForeground(0, b);
		}

		else {
			m_Network->sendUserRequest(username, NetManager::BlockRequest);

			QBrush b;
			b.setColor(Qt::darkRed);
			item->setForeground(0, b);
		}
	}
}

void MainWindow::onRoomListContextMenu(const QPoint &pos) {
	// create the menu actions
	QAction *joinAct=new QAction(tr("Join Room"), this);
	QAction *refreshAct=new QAction(tr("Refresh"), this);

	// toggle actions
	QTreeWidgetItem *item=ui->roomList->currentItem();
	joinAct->setEnabled((item!=NULL));
	refreshAct->setEnabled(!m_LoggedInUser.isEmpty());

	// prepare the context menu
	QMenu context(tr("Context Menu"), this);
	context.addAction(joinAct);
	context.addAction(refreshAct);

	QAction *result;
	if ((result=context.exec(ui->roomList->mapToGlobal(pos)))) {
		// join the selected room
		if (result==joinAct)
			prepareJoinRoom();

		// refresh the room list
		else if (result==refreshAct)
			m_Network->sendRoomListRefresh();
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

void MainWindow::onNetUserLoggedIn(const QString &username, const NetManager::UserStatus &status) {
	// create a new item for this user
	QTreeWidgetItem *item=new QTreeWidgetItem(ui->userList);
	item->setText(0, username);
	item->setText(1, "0");

	// style the text if needed
	if (status==NetManager::UserFriend) {
		QBrush b;
		b.setColor(Qt::darkGreen);
		item->setForeground(0, b);
	}

	else if (status==NetManager::UserBlocked) {
		QBrush b;
		b.setColor(Qt::darkRed);
		item->setForeground(0, b);
	}

	// store the username
	m_LoggedInUser=username;

	// also update the window title
	setWindowTitle("Tyranny Client - Logged in as "+username);
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
	if (ld.exec()==QDialog::Accepted) {
		m_Network->sendFriendListUpdate(ld.getUserList());

		// make sure to style the user list with this new friend list
		for (int i=0; i<ui->userList->topLevelItemCount(); i++) {
			QTreeWidgetItem *item=ui->userList->topLevelItem(i);

			// don't modify blocked users
			QBrush color=item->foreground(0);
			if (color.color()==Qt::darkRed)
				continue;

			// see if this user is now listed as a friend
			QString username=item->text(0);
			if (ld.getUserList().contains(username)) {
				QBrush b;
				b.setColor(Qt::darkGreen);
				item->setForeground(0, b);
			}

			else {
				QBrush b;
				b.setColor(Qt::black);
				item->setForeground(0, b);
			}
		}
	}
}

void MainWindow::onNetBlockedList(const QStringList &list) {
	UserListDialog ld("Blocked Users", list, this);
	if (ld.exec()==QDialog::Accepted) {
		m_Network->sendBlockedListUpdate(ld.getUserList());

		// make sure to style the user list with this new friend list
		for (int i=0; i<ui->userList->topLevelItemCount(); i++) {
			QTreeWidgetItem *item=ui->userList->topLevelItem(i);

			// don't modify friends
			QBrush color=item->foreground(0);
			if (color.color()==Qt::darkGreen)
				continue;

			// see if this user is now listed as a friend
			QString username=item->text(0);
			if (ld.getUserList().contains(username)) {
				QBrush b;
				b.setColor(Qt::darkRed);
				item->setForeground(0, b);
			}

			else {
				QBrush b;
				b.setColor(Qt::black);
				item->setForeground(0, b);
			}
		}
	}
}

void MainWindow::onNetInfoMessage(const QString &msg) {
	QMessageBox::information(this, tr("Information"), msg, QMessageBox::Ok, QMessageBox::NoButton);
}

void MainWindow::onNetErrorMessage(const QString &msg) {
	QMessageBox::critical(this, tr("Error"), msg, QMessageBox::Ok, QMessageBox::NoButton);
}

void MainWindow::onNetJoinGameServer(int gid, const QString &host, int port) {
	qDebug() << "join server at " << host << ":" << port;

	// open a game window
	m_GameWnd=new GameWindow(gid, m_LoggedInUser, host, port, this);
	m_GameWnd->show();
}

void MainWindow::onNetRoomListUpdate(const RoomData &room) {
	// walk the list of rooms and find the room by its id number
	for (int i=0; i<ui->roomList->topLevelItemCount(); i++) {
		QTreeWidgetItem *item=ui->roomList->topLevelItem(i);
		int id=item->text(0).toInt();
		if (id==room.getGid()) {
			// update the room's attributes
			item->setText(1, room.getOwner());
			item->setText(2, QString("%1").arg(room.getPlayerCount()));

			// translate room status and type into text
			QString status, type;
			switch(room.getStatus()) {
				case RoomData::Open: status="Open"; break;
				case RoomData::InProgress: status="In Progress"; break;
				case RoomData::Closed: status="Closed"; break;
			}

			switch(room.getType()) {
				case RoomData::Public: type="Public"; break;
				case RoomData::Private: type="Private"; break;
			}

			// set these column labels as well
			item->setText(3, status);
			item->setText(4, type);

			return;
		}
	}

	// if we are still here, then the room wasn't found so we create an entry for it
	QTreeWidgetItem *item=new QTreeWidgetItem(ui->roomList);
	item->setText(0, QString("%1").arg(room.getGid()));
	item->setText(1, room.getOwner());
	item->setText(2, QString("%1").arg(room.getPlayerCount()));

	// translate room status and type into text
	QString status, type;
	switch(room.getStatus()) {
		case RoomData::Open: status="Open"; break;
		case RoomData::InProgress: status="In Progress"; break;
		case RoomData::Closed: status="Closed"; break;
	}

	switch(room.getType()) {
		case RoomData::Public: type="Public"; break;
		case RoomData::Private: type="Private"; break;
	}

	item->setText(3, status);
	item->setText(4, type);
}

void MainWindow::onNetRoomListDelete(int gid) {
	// find the target room by id
	for (int i=0; i<ui->roomList->topLevelItemCount(); i++) {
		QTreeWidgetItem *item=ui->roomList->topLevelItem(i);
		if (item->text(0).toInt()==gid)
			delete ui->roomList->takeTopLevelItem(i);
	}
}

void MainWindow::onNetRoomListRefresh(const QVector<RoomData> &list) {
	// clear the room list out
	ui->roomList->clear();

	for (int i=0; i<list.size(); i++) {
		RoomData room=list[i];

		// create a new list entry
		QTreeWidgetItem *item=new QTreeWidgetItem(ui->roomList);
		item->setText(0, QString("%1").arg(room.getGid()));
		item->setText(1, room.getOwner());
		item->setText(2, QString("%1").arg(room.getPlayerCount()));

		// translate room status and type into text
		QString status, type;
		switch(room.getStatus()) {
			case RoomData::Open: status="Open"; break;
			case RoomData::InProgress: status="In Progress"; break;
			case RoomData::Closed: status="Closed"; break;
		}

		switch(room.getType()) {
			case RoomData::Public: type="Public"; break;
			case RoomData::Private: type="Private"; break;
		}

		item->setText(3, status);
		item->setText(4, type);
	}
}

bool MainWindow::eventFilter(QObject *sender, QEvent *e) {
	if (sender==ui->chatEdit && e->type()==QEvent::KeyPress) {
		QKeyEvent *ke=static_cast<QKeyEvent*>(e);
		if (ke->key()==Qt::Key_Return)
			onSendButtonClicked();
	}

	return QMainWindow::eventFilter(sender, e);
}

void MainWindow::prepareJoinRoom() {
	// get the currently selected item
	QTreeWidgetItem *item=ui->roomList->currentItem();

	// determine if its a private room, and if so, prompt for a password
	QString password="";
	if (item->text(4)=="Private")
		password=QInputDialog::getText(this, tr("Authentication"), tr("Enter room password:"), QLineEdit::Password);

	m_Network->sendJoinRoom(item->text(0).toInt(), password);
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
