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
// profiledialog.h: definition of the ProfileDialog class.

#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QDialog>

namespace Ui {
	class ProfileDialog;
}

/**
 * Dialog used to present the user with his/her profile data.
 * User's can have public profiles, and this dialog allows the user to
 * edit their data.
 */
class ProfileDialog: public QDialog {
	Q_OBJECT

	public:
		/**
		 * Creates a profile editor dialog with the given data.
		 *
		 * @param realName The user's real name.
		 * @param email The user's email address.
		 * @param age The user's age.
		 * @param bio The user's biography.
		 * @param parent The parent window for this dialog.
		 */
		ProfileDialog(const QString &realName="", const QString &email="", int age=0, const QString &bio="", QWidget *parent=NULL);

		/**
		 * Return's the inputted real name.
		 *
		 * @return The user's real name.
		 */
		QString getName() const;

		/**
		 * Returns the inputted email address.
		 *
		 * @return The user's email address.
		 */
		QString getEmailAddress() const;

		/**
		 * Returns the inputted age.
		 *
		 * @return The user's age.
		 */
		int getAge() const;

		/**
		 * Returns the inputted biography string.
		 *
		 * @return The user's biography.
		 */
		QString getBiography() const;

	private:
		Ui::ProfileDialog *ui;
};

#endif
