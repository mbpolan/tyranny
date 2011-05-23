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
// mainwindow.h: declaration of the MainWindow class

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include "prefdialog.h"

namespace Ui {
	class MainWindow;
}

/**
 * A class that serves as the controller for the client's main window.
 * This class abstracts the interface for the window that provides the user with
 * a way to interact with the client when he or she is in the lobby. It is responsible
 * for handling all of the related menu actions, widget events, and network communications
 * which require data to be presented to the user. Here is where the list of open rooms is
 * displayed and managed, and where the general chat box resides.
 *
 * The UI file for this class is named mainwindow.ui.
 */
class MainWindow: public QMainWindow {
	Q_OBJECT

	public:
		/**
		 * Default constructor for the MainWindow class.
		 * The parent parameter should always be left as NULL since it is illogical for
		 * this window to have a parent.
		 */
		MainWindow(QWidget *parent=NULL);

		/**
		 * Default destructor for the MainWindow class.
		 */
		~MainWindow();

	private slots:
		void onPreferences();

	private:
		/// Pointer to the user interface object for this class.
		Ui::MainWindow *ui;

		/// Cached preferences data.
		PrefDialog::Data *m_PrefData;
};

#endif
