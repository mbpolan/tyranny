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
// statsdialog.cpp: implementation of the StatsDialog class.

#include <QDebug>
#include "statsdialog.h"

#include "ui/ui_statsdialog.h"

StatsDialog::StatsDialog(int points, int gamesPlayed, int won, int lost, QWidget *parent): QDialog(parent) {
	ui=new Ui::StatsDialog;
	ui->setupUi(this);

	// set the statistics values
	ui->pointsLabel->setText(QString("%1").arg(points));
	ui->playedLabel->setText(QString("%1").arg(gamesPlayed));
	ui->wonLabel->setText(QString("%1").arg(won));
	ui->lostLabel->setText(QString("%1").arg(lost));

	// avoid dividing by zero
	if (gamesPlayed>0) {
		double percent=(double(won)/double(gamesPlayed))*100.0;
		ui->percentLabel->setText(QString("%1%").arg(percent));
	}

	else
		ui->percentLabel->setText("100%");
}
