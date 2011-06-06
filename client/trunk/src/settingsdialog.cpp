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
// settingsdialog.cpp: implementation of the SettingsDialog class.

#include <QMessageBox>

#include "settingsdialog.h"

#include "ui/ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent): QDialog(parent) {
	ui=new Ui::SettingsDialog;
	ui->setupUi(this);

	// connect signals
	connect(ui->changePasswordCB, SIGNAL(toggled(bool)), this, SLOT(onChangePasswordCB(bool)));
}

bool SettingsDialog::didChangePassword() const {
	return ui->changePasswordCB->isChecked();
}

QString SettingsDialog::getNewPassword() const {
	return ui->passwordEdit->text();
}

void SettingsDialog::onChangePasswordCB(bool toggled) {
	ui->passwordEdit->setEnabled(toggled);
	ui->verifyEdit->setEnabled(toggled);
}

void SettingsDialog::accept() {
	// verify that both passwords match if the user requested a new password
	if (ui->changePasswordCB->isChecked()) {
		// disallow blank password
		if (ui->passwordEdit->text()=="") {
			QMessageBox::critical(this, tr("Error"), tr("Your password cannot be blank."), QMessageBox::Ok);
			return;
		}

		else if (ui->passwordEdit->text()!=ui->verifyEdit->text()) {
			QMessageBox::critical(this, tr("Error"), tr("Both passwords must match."), QMessageBox::Ok);
			return;
		}
	}

	QDialog::accept();
}
