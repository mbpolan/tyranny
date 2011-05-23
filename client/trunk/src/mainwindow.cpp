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
// mainwindow.cpp: implementation of the MainWindow class

#include "iohandler.h"
#include "mainwindow.h"
#include "prefdialog.h"

#include "ui/ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
	ui=new Ui::MainWindow;
	ui->setupUi(this);

	// connect signals
	connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(onPreferences()));

	// try to load the preferences file
	QString ip;
	int port;
	QVector<QPair<QString, int> > servers;

	IOHandler io("client.dat");
	if (io.loadPreferences(&ip, &port, &servers))
	    m_PrefData=new PrefDialog::Data(ip, port, servers);
	else
	    m_PrefData=NULL;
}

MainWindow::~MainWindow() {
	if (m_PrefData)
		delete m_PrefData;
}

void MainWindow::onPreferences() {
	PrefDialog pd(m_PrefData, this);
	if (pd.exec()) {
		// we don't need the old data anymore
		if (m_PrefData)
		    delete m_PrefData;

		// update preferences data
		m_PrefData=pd.getPreferencesData();

		// save the new preferences data to file
		IOHandler io("client.dat");
		io.savePreferences(m_PrefData->getIP(), m_PrefData->getPort(), m_PrefData->getServers());
	}
}
