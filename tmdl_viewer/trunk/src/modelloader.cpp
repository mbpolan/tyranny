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
// modelloader.cpp: implementation of the ModelLoader class.

#include <QDebug>
#include <QDataStream>
#include <QFile>

#include "modelloader.h"

// the TMDL file header
typedef struct {
	qint32 objectOffset;
	qint32 groupOffset;
	qint32 faceOffset;
	qint32 tcoordOffset;
	qint32 vertexOffset;
	qint32 materialOffset;
} TMDLHeader;

ModelLoader::ModelLoader(Model *model, QObject *parent): QObject(parent) {
	m_Model=model;
}

bool ModelLoader::loadTMDLFile(const QString &path) {
	// try to open the file
	QFile f(path);
	if (!f.open(QIODevice::ReadOnly))
		return false;

	QDataStream in(&f);

	// read the header first
	TMDLHeader header;
	in >> header.objectOffset;
	in >> header.groupOffset;
	in >> header.faceOffset;
	in >> header.tcoordOffset;
	in >> header.vertexOffset;
	in >> header.materialOffset;

	// read model objects
	f.seek(header.objectOffset);

	// read the count then object data itself
	qint16 count;
	in >> count;
	for (int i=0; i<count; i++)
		m_Model->m_Objects.push_back(Model::Object());


	// skip to group data
	f.seek(header.groupOffset);

	// read the count then group data itself
	in >> count;
	for (int i=0; i<count; i++) {
		Model::Group group;
		qint16 object;

		// parent object, and group material
		in >> object;
		in >> group.material;

		qint16 fcount;
		in >> fcount;
		for (int j=0; j<fcount; j++) {
			qint16 findex;
			in >> findex;

			group.faces.push_back(findex);
		}

		m_Model->m_Groups.push_back(group);
		m_Model->m_Objects[object].groups.push_back(m_Model->m_Groups.size()-1);
	}

	// skip to face data
	f.seek(header.faceOffset);

	// read the count then face data itself
	in >> count;
	for (int i=0; i<count; i++) {
		Model::Face face;
		qint16 group;

		// parent group for this face
		in >> group;

		// we have to read in vertices and tex coords indices
		qint16 vcount, tcount;
		in >> vcount;

		for (int j=0; j<vcount; j++) {
			qint16 vindex;
			in >> vindex;

			face.vertices.push_back(vindex);
		}

		in >> tcount;
		for (int j=0; j<tcount; j++) {
			qint16 tindex;
			in >> tindex;

			face.tcoords.push_back(tindex);
		}

		m_Model->m_Faces.push_back(face);
		m_Model->m_Groups[group].faces.push_back(m_Model->m_Faces.size()-1);
	}

	// skip to texture coordinate data
	f.seek(header.tcoordOffset);

	// read the count and then the tex coords themselves
	in >> count;
	for (int i=0; i<count; i++) {
		Model::TexCoord tc;
		in >> tc.u;
		in >> tc.v;

		m_Model->m_TexCoords.push_back(tc);
	}

	// skip to vertex data
	f.seek(header.vertexOffset);

	// read the count and then the vertices themselves
	in >> count;
	for (int i=0; i<count; i++) {
		Model::Vertex v;
		in >> v.x;
		in >> v.y;
		in >> v.z;

		m_Model->m_Vertices.push_back(v);
	}

	// skip to materials data
	f.seek(header.materialOffset);

	// read the count and the texture data
	in >> count;
	for (int i=0; i<count; i++) {
		Model::Material m;
		m.diffuseMap="";

		// read string length
		qint32 length;
		in >> length;

		for (int j=0; j<length; j++) {
			quint8 ch;
			in >> ch;

			m.diffuseMap+=(char) ch;
		}

		m_Model->m_Materials.push_back(m);
	}

	// we're done
	f.close();
	return true;
}
