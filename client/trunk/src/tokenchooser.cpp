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
// tokenchooser.cpp: implementation of the TokenChooser class.

#include <QSignalMapper>

#include "tokenchooser.h"

#include "ui/ui_tokenchooser.h"

TokenChooser::TokenChooser(const QVector<QString> &usernames, QWidget *parent): QDialog(parent) {
	ui=new Ui::TokenChooser;
	ui->setupUi(this);

	// update the labels
	ui->player1Name->setText(usernames[0]);
	ui->player2Name->setText(usernames[1]);
	ui->player3Name->setText(usernames[2]);
	ui->player4Name->setText(usernames[3]);

	// set images for buttons
	ui->token1->setIcon(QIcon(":/gfx/piece1"));
	ui->token1->setIconSize(QSize(80,56));

	ui->token2->setIcon(QIcon(":/gfx/piece2"));
	ui->token2->setIconSize(QSize(80,56));

	ui->token3->setIcon(QIcon(":/gfx/piece3"));
	ui->token3->setIconSize(QSize(80,56));

	ui->token4->setIcon(QIcon(":/gfx/piece4"));
	ui->token4->setIconSize(QSize(80,56));

	ui->token5->setIcon(QIcon(":/gfx/piece5"));
	ui->token5->setIconSize(QSize(80,56));

	ui->token6->setIcon(QIcon(":/gfx/piece6"));
	ui->token6->setIconSize(QSize(80,56));

	// map button signals
	QSignalMapper *mapper=new QSignalMapper(this);
	mapper->setMapping(ui->token1, 1);
	mapper->setMapping(ui->token2, 2);
	mapper->setMapping(ui->token3, 3);
	mapper->setMapping(ui->token4, 4);
	mapper->setMapping(ui->token5, 5);
	mapper->setMapping(ui->token6, 6);

	// connect signals
	connect(ui->token1, SIGNAL(clicked()), mapper, SLOT(map()));
	connect(ui->token2, SIGNAL(clicked()), mapper, SLOT(map()));
	connect(ui->token3, SIGNAL(clicked()), mapper, SLOT(map()));
	connect(ui->token4, SIGNAL(clicked()), mapper, SLOT(map()));
	connect(ui->token5, SIGNAL(clicked()), mapper, SLOT(map()));
	connect(ui->token6, SIGNAL(clicked()), mapper, SLOT(map()));
	connect(mapper, SIGNAL(mapped(int)), this, SIGNAL(onTokenChosen(int)));

	// by default disable any buttons
	setButtonsEnabled(false);
}

void TokenChooser::setButtonsEnabled(bool enable) {
	ui->groupBox->setEnabled(enable);
}

void TokenChooser::updateUsername(const QString &username, int piece) {
	QString img=QString(":/gfx/piece%1").arg(piece);

	if (ui->player1Name->text()==username)
		ui->player1Img->setPixmap(QPixmap(img));

	else if (ui->player2Name->text()==username)
		ui->player2Img->setPixmap(QPixmap(img));

	else if (ui->player3Name->text()==username)
		ui->player3Img->setPixmap(QPixmap(img));

	else if (ui->player4Name->text()==username)
		ui->player4Img->setPixmap(QPixmap(img));
}

void TokenChooser::onTokenChosen(int piece) {
	setButtonsEnabled(false);

	emit pieceChosen(piece);
}
