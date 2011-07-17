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
// mainwindow.h: definition of the MainWindow class.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "compiler.h"

namespace Ui {
	class MainWindow;
}

class MainWindow: public QMainWindow {
	Q_OBJECT

	public:
		/**
		 * Creates a new main interface window.
		 *
		 * @param parent The parent for this window (usually just NULL).
		 */
		MainWindow(QWidget *parent=NULL);

	public slots:
		/// Handler for displaying an about dialog for this program.
		void onAbout();

		/// Handler for showing the file chooser dialog for input files.
		void onInputBrowse();

		/// Handler for showing the file chooser dialog for output files.
		void onOutputBrowse();

		/// Handler for beginning the compile process.
		void onCompile();

		/// Handler for started signal from Compiler class.
		void onStarted(const QDateTime &beginTime);

		/// Handler for completed signal from Compiler class.
		void onCompleted(const QDateTime &endTime);

		/// Handler for general messages from Compiler class.
		void onMessage(const QString &str);

		/// Handler for error messages from Compiler class.
		void onError(const QString &str);

		/// Handler for warning messages from Compiler class.
		void onWarning(const QString &str);

	private:
		Ui::MainWindow *ui;

		/// Object for compiling the OBJ models.
		Compiler *m_Compiler;

		/// A count of all warnings.
		int m_WarnCount;

		/// Start time of compile process.
		QDateTime m_StartTime;

		/// End time of compile process.
		QDateTime m_EndTime;
};

#endif
