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
// scene.cpp: implementation of the Scene class.

#include <GL/glu.h>
#include <QKeyEvent>

#include "scene.h"

Scene::Scene(QWidget *parent): QGLWidget(parent) {
	m_Model=NULL;
	m_Pitch=m_Yaw=m_Roll=0.0f;
	m_X=m_Y=0.0f;
	m_Zoom=-10.0f;

	setFocusPolicy(Qt::StrongFocus);
}

void Scene::setModel(Model *model) {
	// delete any previous models
	if (m_Model)
		delete m_Model;

	m_Model=model;
	updateGL();
}

void Scene::setPitch(GLfloat angle) {
	m_Pitch=angle;
	updateGL();
}

void Scene::setYaw(GLfloat angle) {
	m_Yaw=angle;
	updateGL();
}

void Scene::setRoll(GLfloat angle) {
	m_Roll=angle;
	updateGL();
}

void Scene::initializeGL() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, width(), height());

	// set up a perspective
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, GLfloat(width())/GLfloat(height()), 0.1f, 500.0f);

	glMatrixMode(GL_MODELVIEW);
}

void Scene::resize(int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f, GLfloat(w)/GLfloat(h), 0.1f, 500.0f);

    glMatrixMode(GL_MODELVIEW);
}

void Scene::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// move and rotate the scene as needed
	glTranslatef(m_X, m_Y, m_Zoom);
	glRotatef(m_Roll, 1.0f, 0.0f, 0.0f);
	glRotatef(m_Pitch, 0.0f, 1.0f, 0.0f);
	glRotatef(m_Yaw, 0.0f, 0.0f, 1.0f);

	// draw the model, if any
	if (m_Model)
		m_Model->render();
}

void Scene::keyPressEvent(QKeyEvent *e) {
	const GLfloat delta=0.3f;

	if (e->key()==Qt::Key_Down) {
		m_Zoom-=delta;
		updateGL();
	}

	else if (e->key()==Qt::Key_Up) {
		m_Zoom+=delta;
		updateGL();
	}

	else if (e->key()==Qt::Key_W) {
		m_Y+=delta;
		updateGL();
	}

	else if (e->key()==Qt::Key_S) {
		m_Y-=delta;
		updateGL();
	}

	else if (e->key()==Qt::Key_A) {
		m_X-=delta;
		updateGL();
	}

	else if (e->key()==Qt::Key_D) {
		m_X+=delta;
		updateGL();
	}
}
