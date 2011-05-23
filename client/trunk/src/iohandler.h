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
