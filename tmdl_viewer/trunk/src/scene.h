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
// scene.h: definition of the Scene class.

#ifndef SCENE_H
#define SCENE_H

#include <QGLWidget>

#include "model.h"

/**
 * An OpenGL rendered scene for displaying Tyranny client models.
 * This class also provides functionality for manipulating the scene, such
 * as rotations, translations, etc. Initially, the scene will be blank until
 * a call to setModel() is made.
 */
class Scene: public QGLWidget {
	Q_OBJECT

	public:
		/**
		 * Creates a blank scene widget.
		 *
		 * @param parent The parent widget for this class.
		 */
		Scene(QWidget *parent=NULL);

		/**
		 * Sets the model to render.
		 *
		 * @param model The model for this scene.
		 */
		void setModel(Model *model);

		/**
		 * Sets the pitch for the scene.
		 *
		 * @param angle The angle of the pitch.
		 */
		void setPitch(GLfloat angle);

		/**
		 * Sets the yaw for the scene.
		 *
		 * @param angle The angle of the yaw.
		 */
		void setYaw(GLfloat angle);

		/**
		 * Sets the roll for the scene.
		 *
		 * @param angle The angle of the pitch.
		 */
		void setRoll(GLfloat angle);

	protected:
		/// Implements initializing the OpenGL scene.
		void initializeGL();

		/// Handles resizing the scene.
		void resize(int w, int h);

		/// Draws the actual scene with model.
		void paintGL();

		/// Handles keyboard events.
		void keyPressEvent(QKeyEvent *);

	private:
		/// The Tyranny model to display.
		Model *m_Model;

		/// Translation value on x.
		GLfloat m_X;

		/// Translation value on y.
		GLfloat m_Y;

		/// Zooming value.
		GLfloat m_Zoom;

		/// Pitch angle.
		GLfloat m_Pitch;

		/// Yaw angle.
		GLfloat m_Yaw;

		/// Roll angle.
		GLfloat m_Roll;
};

#endif
