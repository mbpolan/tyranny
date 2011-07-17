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
// converter.cpp: implementation of the Converter class.

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QVector>

#include "compiler.h"
#include "tmdl.h"

Compiler::Compiler(const QString &input, const QString &output, QObject *parent): QObject(parent) {
	m_Input=input;
	m_Output=output;
}

void Compiler::start() {
	// try to open the two files
	QFile in(m_Input);
	if (!in.open(QIODevice::ReadOnly)) {
		emit error("Unable to open input file! Check your path.");
		return;
	}

	// temp variables to hold data
	QStringList mtllibs;

	emit started(QDateTime::currentDateTime());

	// state machine variables
	int curGroup=-1;
	int curObject=-1;

	// start reading the input file
	bool success=true;
	while(!in.atEnd() && success) {
		QString line(in.readLine());
		QString id=line.left(2);
		QStringList comp=line.split(' ', QString::SkipEmptyParts);

		// skip comments
		if (line.isEmpty() || line[0]=='#')
			continue;

		// strip newline characters
		if (comp[comp.size()-1].contains('\n')) {
			QString tmp=comp[comp.size()-1];
			int pos=tmp.indexOf('\n');
			if (pos>-1)
				tmp=tmp.left(pos-1);

			comp[comp.size()-1]=tmp;
		}

		// material library
		if (line.contains("mtllib"))
			mtllibs.append(comp[1]);

		// vertex data
		else if (id=="v ") {
			Vertex v;
			if (comp.size()<4) {
				success=false;

				QString msg=QString("Missing components for vertex #%1").arg(m_Vertices.size());
				emit error(msg);
			}

			else {
				v.x=comp[1].toFloat();
				v.y=comp[2].toFloat();
				v.z=comp[3].toFloat();

				m_Vertices.push_back(v);

				emit message(QString("Parsed vertex #%1").arg(m_Vertices.size()-1));
			}
		}

		// texture coordinate data
		else if (id=="vt") {
			TexCoord tc;
			if (comp.size()<3) {
				success=false;

				QString msg=QString("Missing components for texture coordinate #%1").arg(m_Tcoords.size());
				emit error(msg);
			}

			else {
				tc.u=comp[1].toFloat();
				tc.v=comp[2].toFloat();

				m_Tcoords.push_back(tc);

				emit message(QString("Parsed texture coordinate #%1").arg(m_Tcoords.size()-1));
			}
		}

		// group data
		else if (id=="g ") {
			Group g;
			if (comp.size()<2) {
				success=false;

				QString msg=QString("Missing components for group #%1").arg(m_Groups.size());
				emit error(msg);
			}

			else {
				// determine if we have a parent object for this group
				if (curObject==-1) {
					// create one
					Object o;
					o.name="Default";
					m_Objects.push_back(o);

					// and flag it as our working object
					curObject=0;

					emit warning("Generating default object for group set.");
				}

				g.name=comp[1];

				// flag this as our working group
				curGroup=m_Groups.size();
				m_Groups.push_back(g);
			}
		}

		// face data
		else if (id=="f ") {
			Face f;
			if (comp.size()<4) {
				success=false;

				QString msg=QString("Missing components for face #%1").arg(m_Faces.size());
				emit error(msg);
			}

			else {
				// determine if we have a parent group for this face
				if (curGroup==-1) {
					// create one
					Group g;
					g.name="Default";
					m_Groups.push_back(g);

					// and flag it as our working group
					curGroup=0;

					emit warning("Generating default group for face set.");
				}

				// preallocate space
				f.group=curGroup;
				f.vertices=QVector<short>(comp.size()-1);
				f.tcoords=QVector<short>(comp.size()-1);

				// a face can have multiple faces, hence we parse them all
				for (int i=1; i<comp.size(); i++) {
					// a face can have different vertex/t-coord/normal configurations
					QString unit=comp[i];

					// see if there is a texture coordinate following the vertex data
					int pos=unit.indexOf("/");
					if (pos>-1) {
						// the first part is the vertex index
						f.vertices[i-1]=unit.left(pos).toShort()-1;

						// see if there is another forward slash
						if (unit[pos+1]!='/')
							f.tcoords[i-1]=unit.mid(pos+1, unit.indexOf("/", pos+1)-pos-1).toShort()-1;
						else
							f.tcoords[i-1]=-1;
					}

					else {
						// the entire string is the vertex index
						f.vertices[i-1]=unit.toShort()-1;
						f.tcoords[i-1]=-1;
					}

				}

				m_Faces.push_back(f);
				m_Groups[curGroup].faces.push_back(m_Faces.size()-1);
				emit message(QString("Parsed face #%1 of group #%2").arg(m_Faces.size()-1).arg(curGroup));
			}
		}
	}

	// the geometry file is parsed, now parse any material files
	in.close();

	QFileInfo info(m_Input);
	QString base=info.absolutePath();

	// parse each material file
	QString emsg;
	for (int i=0; i<mtllibs.size(); i++) {
		QString path=base+"/"+mtllibs[i];

		// handle errors appropriately
		if (!parseMaterialLibrary(path, m_Materials, emsg)) {
			emit error(emsg);
			success=false;

			break;
		}
	}

	// assuming everything was parsed successfully, compile the final model file
	if (success) {
		emit message("Beginning write of final model file...");

		if (!writeModelData(emsg))
			emit error(emsg);

		emit message("Final model successfully compiled");
	}

	emit completed(QDateTime::currentDateTime());
}

bool Compiler::parseMaterialLibrary(const QString &file, QVector<Material> &materials, QString &error) {
	QFile mtl(file);
	if (!mtl.open(QIODevice::ReadOnly)) {
		error="Unable to open material library file: \""+file+"\"";
		return false;
	}

	// the material we are currently working with
	int curMaterial=-1;

	// parse the file line by line
	while(!mtl.atEnd()) {
		QString line(mtl.readLine());
		QStringList comp=line.split(' ', QString::SkipEmptyParts);

		// skip comments and blank lines
		if (line.isEmpty() || line[0]=='#')
			continue;

		// strip newline characters
		if (comp[comp.size()-1].contains('\n')) {
			QString tmp=comp[comp.size()-1];
			int pos=tmp.indexOf('\n');
			if (pos>-1)
				tmp=tmp.left(pos-1);

			comp[comp.size()-1]=tmp;
		}

		// beginning of a material section
		if (line.contains("newmtl")) {
			materials.push_back(Material());
			curMaterial=materials.size()-1;
		}

		// diffuse texture map
		else if (line.contains("map_Kd")) {
			if (curMaterial==-1) {
				error="Missing parent material for texture map.";
				return false;
			}

			materials[curMaterial].diffuseTexture=comp[1];
		}
	}

	mtl.close();
	return true;
}

bool Compiler::writeModelData(QString &error) {
	// try to open the output file
	QFile f(m_Output);
	if (!f.open(QIODevice::WriteOnly)) {
		error="Unable to open output file! Check your path.";
		return false;
	}

	QDataStream out(&f);

	// we will fill this in as we go
	TMDLHeader header={ 0, 0, 0, 0, 0, 0 };
	out << (qint32) header.objectOffset << (qint32) header.groupOffset << (qint32) header.faceOffset
	    << (qint32) header.tcoordOffset << (qint32) header.vertexOffset << (qint32) header.materialOffset;

	// we start by writing vertex data
	header.vertexOffset=f.pos();
	int count=m_Vertices.size();
	out << (qint16) count;

	emit message("Writing vertex data...");

	for (int i=0; i<m_Vertices.size(); i++) {
		Vertex v=m_Vertices[i];
		out << (float) v.x << (float) v.y << (float) v.z;
	}

	// follow up by texture coordinate data
	header.tcoordOffset=f.pos();
	count=m_Tcoords.size();
	out << (qint16) count;

	emit message("Writing texture coordinate data...");

	for (int i=0; i<m_Tcoords.size(); i++) {
		TexCoord tc=m_Tcoords[i];
		out << (float) tc.u << (float) tc.v;
	}

	// write faces data
	header.faceOffset=f.pos();
	count=m_Faces.size();
	out << (qint16) count;

	emit message("Writing polygon face data...");

	for (int i=0; i<m_Faces.size(); i++) {
		Face f=m_Faces[i];
		out << (qint16) f.group << (qint16) f.vertices.size();

		// write all face vertex indices
		for (int j=0; j<f.vertices.size(); j++)
			out << (qint16) f.vertices[j];

		out << (qint16) f.tcoords.size();
		for (int j=0; j<f.tcoords.size(); j++)
			out << (qint16) f.tcoords[j];
	}

	// write groups data
	header.groupOffset=f.pos();
	count=m_Groups.size();
	out << (qint16) count;

	emit message("Writing group data...");

	for (int i=0; i<m_Groups.size(); i++) {
	    Group g=m_Groups[i];
	    out << (qint16) g.object << (qint16) g.material << (qint16) g.faces.size();

	    for (int j=0; j<g.faces.size(); j++)
		out << (qint16) g.faces[j];
	}

	// write object data
	header.objectOffset=f.pos();
	count=m_Objects.size();
	out << (qint16) count;

	// write material data
	header.materialOffset=f.pos();
	count=m_Materials.size();
	out << (qint16) count;

	emit message("Writing materials data...");

	for (int i=0; i<m_Materials.size(); i++) {
		Material m=m_Materials[i];
		out << (qint32) m.diffuseTexture.size();

		for (int j=0; j<m.diffuseTexture.size(); j++)
			out << (char) m.diffuseTexture[j].toAscii();
	}

	// now write the header
	f.seek(0);
	out << (qint32) header.objectOffset << (qint32) header.groupOffset << (qint32) header.faceOffset
	    << (qint32) header.tcoordOffset << (qint32) header.vertexOffset << (qint32) header.materialOffset;

	f.close();
	return true;
}
