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
// prefdialog.h: definition of the PrefDialog class.

#ifndef PREFDIALOG_H
#define PREFDIALOG_H

#include <QDialog>

namespace Ui {
	class PrefDialog;
}

/**
 * A dialog that allows users to modify connection details, and store a list of saved servers.
 * Users can modify which server to connect to by this dialog. It presents the user with an
 * interface that allows them to set connection details for their target server, and it also
 * lets the user save a list of favorite servers to connect to at a later time.
 *
 * The UI file for this class is named prefdialog.ui.
 */
class PrefDialog: public QDialog {
	Q_OBJECT

	public:
		/**
		 * Class that stores data inputted into the Preferences dialog by the user.
		 */
		class Data {
			public:
				/**
				 * Default constructor for this class.
				 * @param ip The target server IP.
				 * @param port The target server port.
				 * @param servers A list of saved servers.
				 */
				Data(const QString &ip, int port, const QVector<QPair<QString,int> > &servers):
				    m_IP(ip), m_Port(port), m_Servers(servers) { }

				/**
				 * Returns the IP address of the target server.
				 * @return The target server's IP address.
				 */
				QString getIP() const { return m_IP; }

				/**
				 * Returns the port number of the target server.
				 * @return The target server's port number.
				 */
				int getPort() const { return m_Port; }

				/**
				 * Returns a list of saved servers.
				 * @return Vector of IP address and port pairs.
				 */
				QVector<QPair<QString, int> > getServers() const { return m_Servers; }

			private:
				/// IP address for the target server.
				QString m_IP;

				/// Port number of the target server.
				int m_Port;

				/// List of saved servers.
				QVector<QPair<QString, int> > m_Servers;
		};

	public:
		/**
		 * Default constructor for the Preferences dialog.
		 * A pointer to a Data object may be passed, in which case the dialog will be populated
		 * with the data contained in it. Otherwise, passing NULL will leave the dialog blank.
		 *
		 * @param data Data to populate the dialog with, or NULL to leave all fields blank.
		 * @param parent The parent window for this dialog.
		 */
		PrefDialog(PrefDialog::Data *data, QWidget *parent=NULL);

		/**
		 * Returns the data synthesized from the dialog widgets.
		 *
		 * @return A Data object populated with the fields from the dialog.
		 */
		PrefDialog::Data* getPreferencesData() const;

	private:
		Ui::PrefDialog *ui;
};

#endif
