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
// settingsdialog.h: definition of the SettingsDialog class.

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
	class SettingsDialog;
}

/**
 * A dialog for editing account settings.
 * New passwords and other account related data can be edited by the user
 * and this dialog presents them in a friendly way. The dialog also verifies
 * that the new password the user wants has been verified by retyping.
 */
class SettingsDialog: public QDialog {
	Q_OBJECT

	public:
		/**
		 * Creates a new dialog for editing account settings.
		 *
		 * @param parent The parent widget for this dialog.
		 */
		SettingsDialog(QWidget *parent=NULL);

		/**
		 * Checks whether or not the user requested a new password.
		 *
		 * @return True if yes, false otherwise.
		 */
		bool didChangePassword() const;

		/**
		 * Returns the new password for the user.
		 *
		 * @return A new, verified password.
		 */
		QString getNewPassword() const;

	private slots:
		/// Handler for clicks on the "Change your password" check box.
		void onChangePasswordCB(bool toggled);

	public slots:
		virtual void accept();

	private:
		Ui::SettingsDialog *ui;
};

#endif
