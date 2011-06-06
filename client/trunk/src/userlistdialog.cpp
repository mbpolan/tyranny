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
// userlistdialog.cpp: implementation of the UserListDialog class.

#include "userlistdialog.h"

#include "ui/ui_userlistdialog.h"

UserListDialog::UserListDialog(const QString &title, const QStringList &usernames, QWidget *parent): QDialog(parent) {
	ui=new Ui::UserListDialog;
	ui->setupUi(this);

	// connect signals
	connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(onRemove()));

	// modify dialog title and group box title
	setWindowTitle(title);
	ui->groupBox->setTitle(title);

	// populate the list
	ui->list->addItems(usernames);
}

QStringList UserListDialog::getUserList() const {
	QStringList users;

	for (int i=0; i<ui->list->count(); i++) {
		QListWidgetItem *item=ui->list->item(i);
		users.append(item->text());
	}

	return users;
}

void UserListDialog::onRemove() {
	QListWidgetItem *item=ui->list->currentItem();
	if (item) {
		// remove this item
		ui->list->takeItem(ui->list->currentRow());
		delete item;
	}
}
