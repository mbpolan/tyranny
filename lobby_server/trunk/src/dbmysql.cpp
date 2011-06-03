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
// dbmysql.cpp: implementation of the DBMySQL class.

#include <cstring>

#include "dbmysql.h"

DBMySQL::DBMySQL(const std::string &host, int port, const std::string &db) {
	m_Host=host;
	m_Port=port;
	m_Database=db;
	m_Handle=NULL;

	// initialize the mysql client library
	mysql_library_init(0, NULL, NULL);
}

DBMySQL::~DBMySQL() {
	// close any current connections
	if (m_Handle)
		mysql_close(m_Handle);

	mysql_library_end();
}

void DBMySQL::connect(const std::string &user, const std::string &password) throw(DBMySQL::Exception) {
	// create a handle
	if (!(m_Handle=mysql_init(NULL))) {
		m_Handle=NULL;
		throw DBMySQL::Exception("Unable to allocate MySQL handle (1).");
	}

	// try to connect to the server
	if (!mysql_real_connect(m_Handle, m_Host.c_str(), user.c_str(), password.c_str(),
					   m_Database.c_str(), m_Port, NULL, 0)) {
		mysql_close(m_Handle);
		m_Handle=NULL;

		throw DBMySQL::Exception("Unable to connect to MySQL server.");
	}
}

void DBMySQL::disconnect() throw(DBMySQL::Exception) {
	if (!m_Handle)
		throw DBMySQL::Exception("There is no current connection.");

	mysql_close(m_Handle);
	m_Handle=NULL;
}

bool DBMySQL::authenticate(const std::string &username, const std::string &password) throw(DBMySQL::Exception) {
	if (!m_Handle)
		throw DBMySQL::Exception("There is no current connection.");

	// form the sql string
	std::string sql="SELECT uid FROM users WHERE username='"+username+"' AND password='"+password+"'";

	// and query the server
	if (mysql_query(m_Handle, sql.c_str()))
		throw DBMySQL::Exception("Unable to complete database query: "+std::string(mysql_error(m_Handle)));

	// grab the results
	MYSQL_RES *result=mysql_store_result(m_Handle);
	MYSQL_ROW row=mysql_fetch_row(result);
	bool success=(row ? true : false);

	// clean up
	mysql_free_result(result);

	return success;
}
