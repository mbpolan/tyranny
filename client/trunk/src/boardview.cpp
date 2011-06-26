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
// boardview.cpp: implementation of the BoardView class.

#include <cmath>
#include <QDebug>
#include <QKeyEvent>

#include "boardview.h"

BoardView::BoardView(QWidget *parent): QGLWidget(parent) {
	// set some smart defaults
	m_Roll=-33.0f;
	m_Pitch=45.0f;
	m_X=0.0f;
	m_Y=-3.0f;
	m_Z=-8.0f;
	m_RenderMode=Normal;

	// create the texture array
	m_Textures=QVector<GLuint>(BoardView::NTextures);

	// cache indexes for real-estate properties
	m_PropIndex={1, 3, 6, 8, 9, 11, 13, 14, 16, 18, 19, 21, 23, 24, 26, 27, 29, 31, 32, 34, 37, 39 };

	setFocus();
	grabKeyboard();
}

void BoardView::initializeGL() {
	// prepare the opengl state machine with some basic settings
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	// enable lighting for a nicer effect
	GLfloat position0[]={ 0.0f, 0.0f, 1.0f, 0.0f };
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, position0);

	// load all necessary textures
	glEnable(GL_TEXTURE_2D);
	loadTextures();
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLdouble) width()/(GLdouble) height(), 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
}

void BoardView::resizeGL(int w, int h) {
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLdouble) w/ (GLdouble) h, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
}

void BoardView::paintGL() {
	// re-enable automatic buffer swapping if its disabled
	if (!autoBufferSwap())
		setAutoBufferSwap(true);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// prepare the scene
	glTranslatef(m_X, m_Y, m_Z);
	glRotatef(m_Roll, 1.0f, 0.0f, 0.0f);
	glRotatef(m_Pitch, 0.0f, 0.0f, 1.0f);

	// draw the board
	renderBoard();

	// see if we are in selection mode
	if (m_RenderMode==Selection) {
	    m_RenderMode=Normal;

	    // find out what space was selected
	    handleSelection();

	    // we need to redraw the scene since we are drawing to the back buffer
	    updateGL();
	}
}

void BoardView::mousePressEvent(QMouseEvent *e) {
    	// calculate the current mouse position, set selection mode, and
	// disable automatic buffer swapping
	m_RenderMode=Selection;
	m_Cursor=e->posF();
	setAutoBufferSwap(false);

	updateGL();
}

void BoardView::keyPressEvent(QKeyEvent *e) {
	const GLdouble tdelta=0.10;
	const GLdouble rdelta=0.75;

	// zoom out
	if (e->key()==Qt::Key_1) {
		m_Z-=tdelta;
		updateGL();

		emit translationChanged(m_X, m_Y, m_Z);
	}

	// zoom in
	else if (e->key()==Qt::Key_2) {
		m_Z+=tdelta;
		updateGL();

		emit translationChanged(m_X, m_Y, m_Z);
	}

	// move the board up
	else if (e->key()==Qt::Key_W) {
		m_Y+=tdelta;
		updateGL();

		emit translationChanged(m_X, m_Y, m_Z);
	}

	// move the board down
	else if (e->key()==Qt::Key_S) {
		m_Y-=tdelta;
		updateGL();

		emit translationChanged(m_X, m_Y, m_Z);
	}

	// move the board left
	else if (e->key()==Qt::Key_A) {
		m_X-=tdelta;
		updateGL();

		emit translationChanged(m_X, m_Y, m_Z);
	}

	// move the board right
	else if (e->key()==Qt::Key_D) {
		m_X+=tdelta;
		updateGL();

		emit translationChanged(m_X, m_Y, m_Z);
	}

	// increase the pitch
	else if (e->key()==Qt::Key_Right) {
		m_Pitch+=rdelta;
		updateGL();

		emit perspectiveChanged(m_Roll, m_Pitch);
	}

	// decrease the pitch
	else if (e->key()==Qt::Key_Left) {
		m_Pitch-=rdelta;
		updateGL();

		emit perspectiveChanged(m_Roll, m_Pitch);
	}

	// increase the roll
	else if (e->key()==Qt::Key_Up) {
		m_Roll+=rdelta;
		updateGL();

		emit perspectiveChanged(m_Roll, m_Pitch);
	}

	// decrease the roll
	else if (e->key()==Qt::Key_Down) {
		m_Roll-=rdelta;
		updateGL();

		emit perspectiveChanged(m_Roll, m_Pitch);
	}
}

void BoardView::loadTextures() {
	QString files[]={ "purple.png", "lblue.png", "lpurple.png", "orange.png", "red.png",
				"yellow.png", "green.png", "blue.png", "chance.png", "chest.png",
				"luxury.png", "income.png", "rail.png", "util1.png", "util2.png",
				"go.png", "visiting.png", "park.png", "jail.png", "center.png" };

	for (int i=0; i<BoardView::NTextures; i++) {
		if (!processTexture(files[i], i))
			qDebug() << "Unable to load texture: " << files[i];
	}
}

bool BoardView::processTexture(const QString &file, int i) {
	// load the image first
	QImage img, temp;
	if (!img.load(QString("gfx/")+file))
		return false;

	// now convert it to an OpenGL format and create a texture
	temp=QGLWidget::convertToGLFormat(img);
	glGenTextures(1, &m_Textures[i]);
	glBindTexture(GL_TEXTURE_2D, m_Textures[i]);

	// for now we only use linear filtering
	glTexImage2D(GL_TEXTURE_2D, 0, 3, temp.width(), temp.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, temp.bits());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glDisable(GL_ALPHA_TEST);

	return true;
}

void BoardView::handleSelection() {
	GLint vp[4];
	GLubyte color[3];

	glGetIntegerv(GL_VIEWPORT, vp);
	glReadPixels(m_Cursor.x(), vp[3]-m_Cursor.y(), 1, 1, GL_RGB, GL_UNSIGNED_BYTE, color);

	// we only care about the red pixel
	if (color[1]==0 && color[2]==0)
		emit boardSpaceSelected(color[0]/5);
}

GLuint BoardView::selectTexture(int index) {
	// see if this is a collectable property
	switch(index) {
		// dark purple properties
		case 1:
		case 3: return m_Textures[DarkPurple]; break;

		// income tax
		case 4: return m_Textures[IncomeTax]; break;

		// luxury tax
		case 38: return m_Textures[LuxuryTax]; break;

		// community chests
		case 2:
		case 17:
		case 33: return m_Textures[Chest]; break;

		// transportation spaces
		case 5:
		case 15:
		case 25:
		case 35: return m_Textures[Transport]; break;

		// chance
		case 7:
		case 22:
		case 36: return m_Textures[Chance]; break;

		// light blue properties
		case 6:
		case 8:
		case 9: return m_Textures[LightBlue]; break;

		// light purple properties
		case 11:
		case 13:
		case 14: return m_Textures[LightPurple]; break;

		// orange properties
		case 16:
		case 18:
		case 19: return m_Textures[Orange]; break;

		// red properties
		case 21:
		case 23:
		case 24: return m_Textures[Red]; break;

		// yellow properties
		case 26:
		case 27:
		case 29: return m_Textures[Yellow]; break;

		// utilities
		case 12: return m_Textures[Utility1]; break;
		case 28: return m_Textures[Utility2]; break;

		// green properties
		case 31:
		case 32:
		case 34: return m_Textures[Green]; break;

		// blue properties
		case 37:
		case 39: return m_Textures[DarkBlue]; break;
	}

	return 0;
}

void BoardView::beginTexturing(GLuint id) {
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);

	glEnable(GL_TEXTURE_2D);

	if (id>0)
		glBindTexture(GL_TEXTURE_2D, id);

	else {
		glDisable(GL_TEXTURE_2D);
		glColor3f(1.0f, 1.0f, 1.0f);
	}
}

void BoardView::endTexturing() {
	glPopAttrib();
}

void BoardView::beginColoring(int i) {
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor3ub(i*5, 0, 0);
}

void BoardView::endColoring() {
	glPopAttrib();
}

void BoardView::rotateTexture(GLdouble angle) {
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();

	glRotatef(angle, 0.0f, 0.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
}

void BoardView::restoreTexture() {
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void BoardView::renderBoard() {
	// cache geometric constants
	GLdouble h=0.15;	    // the height of the board's 3d prism

	// calculate half of the width of the board
	GLdouble bwh=fabs(((GLdouble) width()-2.0f)/100.0f);

	// calculate the width and height of each regular (non-corner) space
	GLdouble sh=fabs(bwh/8.0f);
	GLdouble sw=fabs((bwh-(sh*2))/9.0f);

	// render the bottom row of properties
	renderRow(bwh, sw, sh, Bottom);
	renderRow(bwh, sw, sh, Left);
	renderRow(bwh, sw, sh, Right);
	renderRow(bwh, sw, sh, Top);

	// begin drawing untextured faces
	glColor3ub(233, 229, 218);

	// draw the center quad
	beginTexturing(m_Textures[Center]);
	glBegin(GL_QUADS);

	glVertex3f(sh, sh, 0.0f);		glTexCoord2f(0.0f, 1.0f);
	glVertex3f(bwh-sh, sh, 0.0f);		glTexCoord2f(1.0f, 1.0f);
	glVertex3f(bwh-sh, bwh-sh, 0.0f);	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(sh, bwh-sh, 0.0f);		glTexCoord2f(0.0f, 0.0f);

	glEnd();
	endTexturing();

	glBegin(GL_QUADS);

	// draw the lower face
	glVertex3f(0.0f, 0.0f, -h);
	glVertex3f(bwh,  0.0f, -h);
	glVertex3f(bwh,  bwh, -h);
	glVertex3f(0.0f, bwh, -h);

	// draw the front face
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f( bwh, 0.0f, 0.0f);
	glVertex3f( bwh, 0.0f, -h);
	glVertex3f(0.0f, 0.0f, -h);

	// draw the rear face
	glVertex3f(0.0f,  bwh, 0.0f);
	glVertex3f( bwh,  bwh, 0.0f);
	glVertex3f( bwh,  bwh, -h);
	glVertex3f(0.0f,  bwh, -h);

	// draw the left face
	glVertex3f(0.0f,  bwh, -h);
	glVertex3f(0.0f,  bwh, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, -h);

	// draw the right face
	glVertex3f( bwh,  bwh, -h);
	glVertex3f( bwh,  bwh, 0.0f);
	glVertex3f( bwh, 0.0f, 0.0f);
	glVertex3f( bwh, 0.0f, -h);

	glColor3f(1.0f, 1.0f, 1.0f);

	glEnd();
}

void BoardView::renderRow(GLdouble bwh, GLdouble w, GLdouble h, const BoardView::Row &row) {
	GLdouble x, y;
	int bias;
	switch(row) {
		case BoardView::Bottom:	bias=0;  x=bwh-h; y=0; break;
		case BoardView::Top:	bias=20; x=h; y=bwh-h; break;
		case BoardView::Left:	bias=10; x=0.0f; y=h; break;
		case BoardView::Right:  bias=30; x=bwh-h; y=bwh-h; break;
	}

	// draw the bottom two corner spaces
	if (row==BoardView::Bottom) {
		// just visiting space
		if (m_RenderMode==Normal)
			beginTexturing(m_Textures[JustVisiting]);
		else
			beginColoring(0);

		glBegin(GL_QUADS);
		glVertex3f(0.0f, 0.0f, 0.0f);		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(h, 0.0f, 0.0f);		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(h, h, 0.0f);			glTexCoord2f(1.0f, 0.0f);
		glVertex3f(0.0f, h, 0.0f);		glTexCoord2f(0.0f, 0.0f);
		glEnd();

		if (m_RenderMode==Normal) endTexturing();
		else endColoring();

		// GO space
		if (m_RenderMode==Normal) {
			beginTexturing(m_Textures[Go]);
			rotateTexture(90.0f);
		}
		else
			beginColoring(0);

		glBegin(GL_QUADS);
		glVertex3f(bwh-h, 0.0f, 0.0f);	glTexCoord2f(0.0f, 1.0f);
		glVertex3f(bwh, 0.0f, 0.0f);		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(bwh, h, 0.0f);		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(bwh-h, h, 0.0f);		glTexCoord2f(0.0f, 0.0f);
		glEnd();

		if (m_RenderMode==Normal) { restoreTexture(); endTexturing(); }
		else endColoring();
	}

	// draw the upper two corner spaces
	else if (row==BoardView::Top) {
		// free parking space
	    if (m_RenderMode==Normal) {
		    beginTexturing(m_Textures[FreeParking]);
		    rotateTexture(180.0f);
	    }
	    else
		    beginColoring(0);

		glBegin(GL_QUADS);
		glVertex3f(0.0f, bwh-h, 0.0f);	glTexCoord2f(0.0f, 1.0f);
		glVertex3f(h, bwh-h, 0.0f);		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(h, bwh, 0.0f);		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(0.0f, bwh, 0.0f);		glTexCoord2f(0.0f, 0.0f);
		glEnd();

		if (m_RenderMode==Normal) { restoreTexture(); endTexturing(); }
		else endColoring();

		// go to jail space
		if (m_RenderMode==Normal)
			beginTexturing(m_Textures[GoToJail]);
		else
			beginColoring(0);

		glBegin(GL_QUADS);
		glVertex3f(bwh-h, bwh-h, 0.0f);	glTexCoord2f(0.0f, 1.0f);
		glVertex3f(bwh, bwh-h, 0.0f);		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(bwh, bwh, 0.0f);		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(bwh-h, bwh, 0.0f);		glTexCoord2f(0.0f, 0.0f);
		glEnd();

		if (m_RenderMode==Normal) endTexturing();
		else endColoring();
	}

	// draw the regular spaces
	for (int i=0; i<9; i++) {
		if (m_RenderMode==Normal) {
			// apply a texture for this space
			beginTexturing(selectTexture(bias+i+1));
		}

		else {
			// determine a color for this space
			beginColoring(bias+i+1);
		}

		// move along right to left
		if (row==BoardView::Bottom) {
			glBegin(GL_QUADS);
			glVertex3f(x, y, 0.0f);		glTexCoord2f(1.0f, 0.0f);
			glVertex3f(x-w, y, 0.0f);	glTexCoord2f(1.0f, 1.0f);
			glVertex3f(x-w, y+h, 0.0f);	glTexCoord2f(0.0f, 1.0f);
			glVertex3f(x, y+h, 0.0f);	glTexCoord2f(0.0f, 0.0f);
			glEnd();

			x-=w;
		}

		// move left to right
		else if (row==BoardView::Top) {
			rotateTexture(180.0f);

			glBegin(GL_QUADS);
			glVertex3f(x, y, 0.0f);		glTexCoord2f(1.0f, 0.0f);
			glVertex3f(x+w, y, 0.0f);	glTexCoord2f(1.0f, 1.0f);
			glVertex3f(x+w, y+h, 0.0f);	glTexCoord2f(0.0f, 1.0f);
			glVertex3f(x, y+h, 0.0f);	glTexCoord2f(0.0f, 0.0f);
			glEnd();

			restoreTexture();

			x+=w;
		}

		// move bottom to top
		else if (row==BoardView::Left) {
			rotateTexture(90.0f);

			glBegin(GL_QUADS);
			glVertex3f(x, y, 0.0f);		glTexCoord2f(1.0f, 0.0f);
			glVertex3f(x+h, y, 0.0f);	glTexCoord2f(1.0f, 1.0f);
			glVertex3f(x+h, y+w, 0.0f);	glTexCoord2f(0.0f, 1.0f);
			glVertex3f(x, y+w, 0.0f);	glTexCoord2f(0.0f, 0.0f);
			glEnd();

			restoreTexture();

			y+=w;
		}

		// move top to bottom
		else if (row==BoardView::Right) {
			rotateTexture(-90.0f);

			glBegin(GL_QUADS);
			glVertex3f(x, y, 0.0f);		glTexCoord2f(1.0f, 0.0f);
			glVertex3f(x+h, y, 0.0f);	glTexCoord2f(1.0f, 1.0f);
			glVertex3f(x+h, y-w, 0.0f);	glTexCoord2f(0.0f, 1.0f);
			glVertex3f(x, y-w, 0.0f);	glTexCoord2f(0.0f, 0.0f);
			glEnd();

			restoreTexture();

			y-=w;
		}

		// stop our current texturing or coloring
		if (m_RenderMode==Normal)
			endTexturing();
		else
			endColoring();
	}
}
