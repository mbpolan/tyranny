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
// dbmysql.h: definition of the DBMySQL class.

#ifndef DBMYSQL_H
#define DBMYSQL_H

#include <iostream>
#include <mysql/mysql.h>

#include "user.h"

class DBMySQL {
	public:
		/**
		 * A general exception for database problems.
		 */
		class Exception {
			public:
				/// Default constructor for socket exceptions.
				Exception(const std::string &msg): m_Message(msg) { };

				/**
				 * Returns the reason for this exception.
				 * @return Message string
				 */
				std::string getMessage() const { return m_Message; }

			private:
				/// The message for this exception.
				std::string m_Message;
		};

	public:
		/**
		 * Creates a new database connection object.
		 *
		 * @param host The database server.
		 * @param port The database port.
		 * @param db The database name.
		 */
		DBMySQL(const std::string &host, int port, const std::string &db);

		/**
		 * Cleans up used resources and frees associated memory.
		 */
		~DBMySQL();

		/**
		 * Connects to the database server and attempts to login with the given information.
		 *
		 * @param user The username.
		 * @param password The password for the user.
		 * @throw An exception describing a connection problem.
		 */
		void connect(const std::string &user, const std::string &password) throw(DBMySQL::Exception);

		/**
		 * Terminates the current connection with the database server.
		 *
		 * @throws An exception describing a disconnect problem.
		 */
		void disconnect() throw(DBMySQL::Exception);

		/**
		 * Tries to authenticate the given user information against the database.
		 *
		 * @param username The username.
		 * @param password The user's password.
		 * @return True if the user is valid and can login, false otherwise.
		 */
		bool authenticate(const std::string &username, const std::string &password) throw(DBMySQL::Exception);

		/**
		 * Gathers and loads a user's data from the database.
		 *
		 * @param user The user to load.
		 * @throw An exception if an error occurred.
		 */
		void loadUser(User *user) throw(DBMySQL::Exception);

	private:
		/// The server address.
		std::string m_Host;

		/// The server port.
		int m_Port;

		/// The database name.
		std::string m_Database;

		/// The MySQL connection handle.
		MYSQL *m_Handle;
};

#endif
