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
// converter.h: definition of the Converter class.

#ifndef CONVERTER_H
#define CONVERTER_H

#include <QDateTime>
#include <QMap>
#include <QObject>

#include "tmdl.h"

class Compiler: public QObject {
	Q_OBJECT

	public:
		/**
		 * Creates a new compiler object.
		 *
		 * @param input The input OBJ model.
		 * @param output The output Tyranny model.
		 * @param parent The parent Qt object.
		 */
		Compiler(const QString &input, const QString &output, QObject *parent=NULL);

		/// Starts the compile process.
		void start();

		/**
		 * Returns the amount of vertices parsed.
		 *
		 * @return Vertex count.
		 */
		int getVertexCount() const { return m_Vertices.size(); }

		/**
		 * Returns the amount of texture coordinates parsed.
		 *
		 * @return Texture coordinate count.
		 */
		int getTextureCoordinateCount() const { return m_Tcoords.size(); }

		/**
		 * Returns the amount of faces parsed.
		 *
		 * @return Face count.
		 */
		int getFaceCount() const { return m_Faces.size(); }

		/**
		 * Returns data about materials parsed.
		 *
		 * @return A vector of Material objects.
		 */
		QVector<Material> getMaterialsData() const { return m_Materials; }

	signals:
		/// Signal emitted when the process has started.
		void started(const QDateTime &beginTime);

		/// Signal emitted when there is a progress message.
		void message(const QString &message);

		/// Signal emitted when an error occurs.
		void error(const QString &message);

		/// Signal emitted when a warning occurs.
		void warning(const QString &message);

		/// Signal emitted when the process has completed.
		void completed(const QDateTime &endTime);

	private:
		/**
		 * Parses a material library file.
		 *
		 * @param file The path to the file.
		 * @param materials Vector, into which new material data will be added.
		 * @param error Will be set to a human readable error message in case of problems.
		 * @return true if no errors occured, false otherwise.
		 */
		bool parseMaterialLibrary(const QString &file, QVector<Material> &materials, QString &error);

		/**
		 * Writes the collected model data into the final output file.
		 * Make sure no errors have occurred while collecting the data.
		 *
		 * @param error Will be set to a human readable error message in case of problems.
		 * @return true if no errors occurred, false otherwise.
		 */
		bool writeModelData(QString &error);

		/// The input OBJ model.
		QString m_Input;

		/// The output Tyranny model.
		QString m_Output;

		/// Collection of model vertices.
		QVector<Vertex> m_Vertices;

		/// Collection of model texture coordinates.
		QVector<TexCoord> m_Tcoords;

		/// Collection of model faces.
		QVector<Face> m_Faces;

		/// Collection of model groups.
		QVector<Group> m_Groups;

		/// Collection of model objects.
		QVector<Object> m_Objects;

		/// Collection of model materials.
		QVector<Material> m_Materials;

		/// Intermediate storage of group indices mapped to material names.
		QMap<short, QString> m_GroupMaterials;
};

#endif
