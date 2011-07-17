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
// tmdl.h: various TMDL file format data.

#ifndef TMDL_H
#define TMDL_H

#include <QString>
#include <QVector>

typedef struct {
	long objectOffset;
	long groupOffset;
	long faceOffset;
	long tcoordOffset;
	long vertexOffset;
	long materialOffset;
} TMDLHeader;

typedef struct {
	float x;
	float y;
	float z;
} Vertex;

typedef struct {
	float u;
	float v;
} TexCoord;

typedef struct {
	short group;
	QVector<short> vertices;
	QVector<short> tcoords;
} Face;

typedef struct {
	short object;
	QString name;
	QVector<int> faces;
	int material;
} Group;

typedef struct {
	QString name;
} Object;

typedef struct {
	QString name;
	QString diffuseTexture;
} Material;

#endif
