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
// gamewindow.cpp: implementation of the GameWindow class.

#include "boardview.h"
#include "gamewindow.h"

#include "ui/ui_gamewindow.h"

GameWindow::GameWindow(const QString &username, QWidget *parent): QMainWindow(parent) {
	ui=new Ui::GameWindow;
	ui->setupUi(this);

	// replace the separator placeholder widget with the board view
	BoardView *view=new BoardView(this);
	ui->glLayout->addWidget(view);
}
