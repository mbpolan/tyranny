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
// boardview.h: definition of the BoardView class.

#ifndef BOARDVIEW_H
#define BOARDVIEW_H

#include <QGLWidget>

class BoardView: public QGLWidget {
	Q_OBJECT

	public:
		/**
		 * Creates a new board viewer widget.
		 *
		 * @param parent The parent for this widget.
		 */
		BoardView(QWidget *parent=NULL);

	protected:
		/// Prepares the OpenGL context.
		void initializeGL();

		/// Handles resize the viewing area.
		void resizeGL(int w, int h);

		/// Handles painting the scene.
		void paintGL();
};

#endif
