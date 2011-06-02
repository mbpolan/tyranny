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
// authdialog.h: definition of the AuthDialog class.

#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

#include <QDialog>

namespace Ui {
	class AuthDialog;
}

/**
 * Dialog presented to the user for authentication.
 * This dialog allows the user to provide a username and a password, which can then
 * be used for various purposes.
 *
 * The UI file for this class is named authdialog.ui.
 */
class AuthDialog: public QDialog {
	Q_OBJECT

	public:
		/// Default constructor
		AuthDialog(QWidget *parent=NULL);

		/**
		 * Returns the inputted username.
		 * @return The provided username.
		 */
		QString getUsername() const;

		/**
		 * Returns the inputted password.
		 * @return The provided password.
		 */
		QString getPassword() const;

	private:
		Ui::AuthDialog *ui;
};

#endif
