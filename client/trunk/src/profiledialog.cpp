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
// profiledialog.cpp: implementation of the ProfileDialog class.

#include "profiledialog.h"

#include "ui/ui_profiledialog.h"

ProfileDialog::ProfileDialog(const QString &realName, const QString &email, int age, const QString &bio, QWidget *parent): QDialog(parent) {
	ui=new Ui::ProfileDialog;
	ui->setupUi(this);

	// set the given data
	ui->nameEdit->setText(realName);
	ui->emailEdit->setText(email);
	ui->ageBox->setValue(age);
	ui->bioEdit->setText(bio);
}

QString ProfileDialog::getName() const {
	return ui->nameEdit->text();
}

QString ProfileDialog::getEmailAddress() const {
	return ui->emailEdit->text();
}

int ProfileDialog::getAge() const {
	return ui->ageBox->value();
}

QString ProfileDialog::getBiography() const {
	return ui->bioEdit->toPlainText();
}
