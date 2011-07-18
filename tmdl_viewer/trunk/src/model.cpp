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
// model.cpp: implementation of the Model class.

#include <QDebug>
#include <QFileInfo>
#include <QGLWidget>
#include <QImage>

#include "model.h"
#include "modelloader.h"

Model::Model(QObject *parent): QObject(parent) {
}

bool Model::load(const QString &path, QStringList &errors) {
	// create a model loader and try to load the data file
	ModelLoader io(this);
	if (io.loadTMDLFile(path)) {
		// we now need to read the image files for the textures
		for (int i=0; i<m_Materials.size(); i++) {
			QString base=QFileInfo(path).absolutePath()+"/";
			base+=m_Materials[i].diffuseMap;

			// attempt to load it
			QImage img, temp;
			if (!img.load(base))
				errors << QString("Unable to load texture image file: ")+base;

			glPushAttrib(GL_ENABLE_BIT);
			glEnable(GL_TEXTURE_2D);

			// convert it to an OpenGL friendly format
			temp=QGLWidget::convertToGLFormat(img);
			glGenTextures(1, &m_Materials[i].diffuseMapID);
			glBindTexture(GL_TEXTURE_2D, m_Materials[i].diffuseMapID);

			// for now we only use linear filtering
			glTexImage2D(GL_TEXTURE_2D, 0, 3, temp.width(), temp.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, temp.bits());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glPopAttrib();
		}

		return true;
	}

	else
	    return false;
}

void Model::render() {
	// the rendering process involves iterating over objects and drawing them, using
	// the group, vertex, tex coord, and face data as we go
	for (int i=0; i<m_Objects.size(); i++) {
		Object obj=m_Objects[i];

		// each object is made up of various groups
		for (int j=0; j<obj.groups.size(); j++) {
			Group grp=m_Groups[obj.groups[j]];
			renderGroup(grp);
		}
	}
}

void Model::renderGroup(const Model::Group &grp) {
	glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT);

	// apply the texture first
	if (grp.material>-1) {
		glEnable(GL_TEXTURE_2D);

		GLuint texture=m_Materials[grp.material].diffuseMapID;
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	else
		glDisable(GL_TEXTURE_2D);

	// draw the faces
	for (int i=0; i<grp.faces.size(); i++) {
		Face face=m_Faces[grp.faces[i]];

		glBegin(GL_POLYGON);

		// faces consist of (possibly) texture mapped polygons
		for (int j=0; j<face.vertices.size(); j++) {
			Vertex v=m_Vertices[face.vertices[j]];
			glVertex3f(v.x, v.y, v.z);

			// see if we need to apply texture coordinates
			if (face.tcoords[j]>-1) {
				TexCoord tc=m_TexCoords[face.tcoords[j]];
				glTexCoord2f(tc.u, tc.v);
			}
		}

		glEnd();
	}

	glPopAttrib();
}
