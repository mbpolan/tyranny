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
// rulesdialog.cpp: implementation of RulesDialog class.

#include "rulesdialog.h"

#include "ui/ui_rulesdialog.h"

RulesDialog::Settings::Settings(int maxTurns, int maxHumans, int freeParkReward, const PropertyRedistribution &redistMethod,
					  bool incomeTaxChoice, const QString &password, bool onlyFriends) {
    m_MaxTurns=maxTurns;
    m_MaxHumans=maxHumans;
    m_FreeParkReward=freeParkReward;
    m_PropRedist=redistMethod;
    m_IncomeTaxChoice=incomeTaxChoice;
    m_Password=password;
    m_OnlyFriends=onlyFriends;
}

/****************************************************************************/

RulesDialog::RulesDialog(QWidget *parent): QDialog(parent) {
	ui=new Ui::RulesDialog;
	ui->setupUi(this);
}

RulesDialog::Settings RulesDialog::getDefinedSettings() const {
	// serialize the data from the dialog
	int maxTurns=ui->turnBox->value();
	int maxHumans=ui->humanPlayerBox->value();
	int freeParkReward=ui->freeParkMoneyCB->value();
	Settings::PropertyRedistribution prop=(ui->propRedistCB->currentText()=="Random to Players" ? Settings::Random : Settings::ReturnToBank);
	bool incomeTaxChoice=ui->incomeTaxCB->isChecked();
	QString password=ui->passwordEdit->text();
	bool onlyFriends=ui->friendsOnlyCB->isChecked();

	return Settings(maxTurns, maxHumans, freeParkReward, prop, incomeTaxChoice, password, onlyFriends);
}
