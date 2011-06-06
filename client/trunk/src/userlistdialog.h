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
// userlistdialog.h: definition of the UserListDialog class.

#ifndef USERLISTDIALOG_H
#define USERLISTDIALOG_H

#include <QDialog>

namespace Ui {
	class UserListDialog;
}

/**
 * A basic dialog that allows the programmer to display a list of strings.
 * This multipurpose dialog can be used to show a list of usernames. It provides
 * a button to remove entries from the list, effective for editing lists of blocked
 * users or friends.
 */
class UserListDialog: public QDialog {
	Q_OBJECT

	public:
		/**
		 * Creates a dialog populated with the given usernames and title.
		 *
		 * @param title The title for the dialog.
		 * @param usernames A list of usernames.
		 * @param parent The parent widget for this dialog.
		 */
		UserListDialog(const QString &title, const QStringList &usernames, QWidget *parent=NULL);

		/**
		 * Returns the list of usernames in the dialog list.
		 *
		 * @return A list of usernames.
		 */
		QStringList getUserList() const;

	private slots:
		/// Handler for Remove button clicks
		void onRemove();

	private:
		Ui::UserListDialog *ui;
};

#endif
