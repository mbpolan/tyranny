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
// modelloader.h: definition of the ModelLoader class.

#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <QObject>

#include "model.h"

/**
 * An I/O class used solely for loading Tyranny client models.
 * The loading code lives in this class specifically for modularization
 * purposes. For example, almost the most common way to store models is
 * in files, it is possible to store the data in other ways, such as part
 * of an encrypted archive or database. As such, this class allows loading
 * the models in more than one way.
 */
class ModelLoader: public QObject {
	Q_OBJECT

	public:
		/**
		 * Creates a model loader for the given model.
		 *
		 * @param model The model, into which loaded data will be placed.
		 * @param parent A parent object for this class.
		 */
		ModelLoader(Model *model, QObject *parent=NULL);

		/**
		 * Attempts to load a Tyranny model from file.
		 *
		 * @param path Path to the model file.
		 * @return true if the model was loaded, false otherwise.
		 */
		bool loadTMDLFile(const QString &path);

	private:
		/// The target model object to place data into.
		Model *m_Model;
};

#endif
