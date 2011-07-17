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

#include "scene.h"

namespace Ui {
	class MainWindow;
}

/**
 * The main interface window.
 * This window provides an OpenGL rendered scene and various controls
 * for viewing and manipulating a given Tyranny client model.
 */
class MainWindow: public QMainWindow {
	Q_OBJECT

	public:
		/**
		 * Creates a new main interface window.
		 *
		 * @param parent The parent for this window, usually NULL.
		 */
		MainWindow(QWidget *parent=NULL);

	private slots:
		/// Handler for opening a model.
		void onOpen();

		/// Handler for viewing information about this program.
		void onAbout();

		/// Handler for adjusting the pitch from slider.
		void onPitchSlider(int val);

		/// Handler for adjusting the yaw from slider.
		void onYawSlider(int val);

		/// Handler for adjusting the roll from slider.
		void onRollSlider(int val);

		/// Handler for adjusting the pitch from spin box.
		void onPitchSpinBox(double val);

		/// Handler for adjusting the yaw from spin box.
		void onYawSpinBox(double val);

		/// Handler for adjusting the roll from spin box.
		void onRollSpinBox(double val);

	private:
		Ui::MainWindow *ui;

		/// The OpenGL rendered view.
		Scene *m_Scene;
};

#endif
