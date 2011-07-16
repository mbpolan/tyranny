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
// turndialog.cpp: implementation of the TurnDialog class.

#include <QPushButton>
#include <QTimer>

#include "turndialog.h"

#include "ui/ui_turndialog.h"

#define TIME_STEP		100
#define ANIM_TIME		3000
#define MAX_LOOPS		int(ANIM_TIME/TIME_STEP)

TurnDialog::TurnDialog(const QVector<QString> &usernames, const QVector<int> &order, QWidget *parent): QDialog(parent) {
	ui=new Ui::TurnDialog;
	ui->setupUi(this);

	// set the labels
	ui->player1Name->setText(usernames[0]);
	ui->player2Name->setText(usernames[1]);
	ui->player3Name->setText(usernames[2]);
	ui->player4Name->setText(usernames[3]);

	// set other variables
	m_TurnOrder=order;
	m_Loops=0;
}

void TurnDialog::animate() {
	m_Loops=0;
	onTick();
}

void TurnDialog::onTick() {
	// if this dialog is not shown, check again in a short while
	if (!isVisible())
		QTimer::singleShot(100, this, SLOT(onTick()));
	else {
		// change the values in the dialog lcd widgets
		ui->player1Num->display((int(ui->player1Num->value())+1)%5);
		ui->player2Num->display((int(ui->player1Num->value())+1)%5);
		ui->player3Num->display((int(ui->player1Num->value())+1)%5);
		ui->player4Num->display((int(ui->player1Num->value())+1)%5);

		// increment iterations counter
		m_Loops++;

		// reanimate if needed
		if (m_Loops<MAX_LOOPS)
			QTimer::singleShot(TIME_STEP, this, SLOT(onTick()));
		else {
			// set the correct turn orders
			for (int i=0; i<4; i++) {
				switch(m_TurnOrder[i]) {
					case 0: ui->player1Num->display(i+1); break;
					case 1: ui->player2Num->display(i+1); break;
					case 2: ui->player3Num->display(i+1); break;
					case 3: ui->player4Num->display(i+1); break;
				}
			}

			// queue the dialog to close
			QTimer::singleShot(2500, this, SLOT(hide()));
		}
	}
}
