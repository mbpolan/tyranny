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

	signals:
		/// Signal emitted when the perspective has changed.
		void perspectiveChanged(GLdouble roll, GLdouble pitce);

		/// Signal emitted when the translation matrix has changed.
		void translationChanged(GLdouble x, GLdouble y, GLdouble z);

		/// Signal emitted when a board space was selected
		void boardSpaceSelected(int i);

	protected:
		/// Prepares the OpenGL context.
		void initializeGL();

		/// Handles resize the viewing area.
		void resizeGL(int w, int h);

		/// Handles painting the scene.
		void paintGL();

		/// Handles mouse click events.
		void mousePressEvent(QMouseEvent *);

		virtual void keyPressEvent(QKeyEvent *);

	private:
		/// Enumeration of different rendering modes.
		enum RenderMode {
			Normal,
			Selection
		};

		/// An enumeration of all internal textures
		enum Textures {
			DarkPurple = 0,
			LightBlue,
			LightPurple,
			Orange,
			Red,
			Yellow,
			Green,
			DarkBlue,
			Chance,
			Chest,
			LuxuryTax,
			IncomeTax,
			Transport,
			Utility1,
			Utility2,
			Go,
			JustVisiting,
			FreeParking,
			GoToJail,
			Center,
			NTextures
		};

		/// An enumeration of rows in the board.
		enum Row {
			Bottom,
			Left,
			Top,
			Right
		};

	private:
		/// Loads necessary widget textures.
		void loadTextures();

		/**
		 * Loads a given texture.
		 *
		 * @param file The filename of the texture.
		 * @param i The index to bind it to.
		 * @return true if successful, false otherwise.
		 */
		bool processTexture(const QString &file, int i);

		/**
		 * Handles dealing with mouse selections.
		 */
		void handleSelection();

		/**
		 * Determines a proper texture for the indexed space.
		 *
		 * @param index The space to examine.
		 * @return A texture id.
		 */
		GLuint selectTexture(int index);

		/**
		 * Applies a texture and sets it as the one to use
		 * for all subsequent drawing commands.
		 *
		 * @param id The texture id.
		 */
		void beginTexturing(GLuint id);

		/**
		 * Undos a call to beginTexturing().
		 * This disables the current texture.
		 */
		void endTexturing();

		/**
		 * Applies a color for the given board space.
		 *
		 * @param i The index for the board space.
		 */
		void beginColoring(int i);

		/**
		 * Undos a call to beginColoring().
		 */
		void endColoring();

		/**
		 * Rotates a texture about the Z-axis.
		 *
		 * @param angle The angle to rotate about.
		 */
		void rotateTexture(GLdouble angle);

		/// Undos previous texture matrix modifications.
		void restoreTexture();

		/// Draws the game board.
		void renderBoard();

		/**
		 * Renders a row of spaces.
		 *
		 * @param bwh The width/height of the board.
		 * @param w The width of each regular (non-corner) space.
		 * @param h The height of each regular (non-corner) space.
		 * @param row Which row to render.
		 */
		void renderRow(GLdouble bwh, GLdouble w, GLdouble h, const BoardView::Row &row);

		/// Perspective.
		GLdouble m_Roll, m_Pitch;

		/// Translation.
		GLdouble m_X, m_Y, m_Z;

		/// Textures.
		QVector<GLuint> m_Textures;

		/// The current rendering mode.
		RenderMode m_RenderMode;

		/// The position of the cursor at the time of a mouse event.
		QPointF m_Cursor;

		/// Indexes for collectable real-estate.
		int m_PropIndex[22];
};

#endif
