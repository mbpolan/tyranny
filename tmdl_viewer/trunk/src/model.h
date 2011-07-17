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
// model.h: definition of the Model class.

#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QString>
#include <QVector>
#include <GL/gl.h>

/**
 * Class for storing and rendering Tyranny client models.
 * Tyranny models are low-polygon objects with optional textures. The Model class
 * is used for representing those models and rendering them onto the screen.
 *
 * This class makes use of the ModelLoader class to parse the model data file,
 * hence all loading procedure can be found there.
 */
class Model: public QObject {
	public:
		/**
		 * Creates an empty model.
		 *
		 * @param parent The parent object for this class.
		 */
		Model(QObject *parent=NULL);

		/**
		 * Loads and initializes data from the given model file.
		 *
		 * @param path The path to the file.
		 * @return true if the model was successfully loaded, false otherwise.
		 */
		bool load(const QString &path);

		/**
		 * Renders the model using the current OpenGL matrix configuration.
		 */
		void render();

	private:
		/// A representation of a point in 3D space.
		typedef struct {
			GLfloat x;
			GLfloat y;
			GLfloat z;
		} Vertex;

		/// A representation of a texture coordinate.
		typedef struct {
			GLfloat u;
			GLfloat v;
		} TexCoord;

		/// A representation of a polygonal face.
		typedef struct {
			QVector<qint16> vertices;
			QVector<qint16> tcoords;
		} Face;

		/// A representation of a group of faces.
		typedef struct {
			qint16 material;
			QVector<qint16> faces;
		} Group;

		/// A representation of a collection of groups.
		typedef struct {
			QVector<qint16> groups;
		} Object;

		/// A representation of a material texture.
		typedef struct {
			QString diffuseMap;
			GLuint diffuseMapID;
		} Material;

	private:
		/**
		 * Renders the given group onto the color buffer.
		 *
		 * @param grp The Group object to render.
		 */
		void renderGroup(const Model::Group &grp);

		/// Collection of all objects that make up this model.
		QVector<Model::Object> m_Objects;

		/// Collection of all groups that make up objects.
		QVector<Model::Group> m_Groups;

		/// Collection of all faces that make up groups.
		QVector<Model::Face> m_Faces;

		/// Collection of all texture coordinates to texture faces.
		QVector<Model::TexCoord> m_TexCoords;

		/// Collection of all vertices.
		QVector<Model::Vertex> m_Vertices;

		/// Collection of materials used by this model.
		QVector<Model::Material> m_Materials;

		friend class ModelLoader;
};

#endif
