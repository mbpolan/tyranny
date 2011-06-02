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
// iohandler.h: definition of the IOHandler class.

#ifndef IOHANDLER_H
#define IOHANDLER_H

#include <QObject>
#include <QString>
#include <QPair>

/**
 * Various serializing methods for multipurpose IO operations.
 * This class provides the functionality to allow the programmer to write/read data to/from
 * files in a standard way. Instead of dealing with low-level file API, the IOHandler class
 * abstracts it on a high level, instead allowing the programmer to pass logical data types
 * to be written/read to/from a file.
 */
class IOHandler: public QObject {
	public:
		/**
		 * Default constructor.
		 *
		 * @param path Path to a file to work on.
		 */
		IOHandler(const QString &path, QObject *parent=NULL);

		/**
		 * Loads preferences data from file.
		 *
		 * @param ip Pointer to a QString to store the target server's IP address.
		 * @param port Pointer to an int to store the target server's port.
		 * @param serverList Pointer to a vector of string-int pairs to store list of saved servers.
		 */
		bool loadPreferences(QString *ip, int *port, QVector<QPair<QString, int> > *serverList);

		/**
		 * Saves preferences data to a file.
		 *
		 * @param ip The IP address of the target server.
		 * @param port The port number of the target server.
		 * @param serverList List of saved servers.
		 */
		bool savePreferences(const QString &ip, int port, const QVector<QPair<QString, int> > &serverList);

	private:
		/// Path to the file to work on.
		QString m_Path;

		/// The file handled by this object.
		FILE *m_File;
};

#endif
