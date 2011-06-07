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
#include <cstdlib>
#include <sstream>

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

void DBMySQL::loadUser(User *user) throw(DBMySQL::Exception) {
	if (!m_Handle)
		throw DBMySQL::Exception("There is no current connection.");

	// form the sql string
	std::string sql="SELECT * FROM users WHERE username='"+user->getUsername()+"'";

	// and query the server
	if (mysql_query(m_Handle, sql.c_str()))
		throw DBMySQL::Exception("Unable to complete database query: "+std::string(mysql_error(m_Handle)));

	// grab the results
	MYSQL_RES *result=mysql_store_result(m_Handle);
	MYSQL_ROW row=mysql_fetch_row(result);

	if (!row) {
		mysql_free_result(result);
		throw DBMySQL::Exception("The given user could not be found in the database.");
	}

	// email address (column indexed 3)
	if (row[3])
		user->setEmail(std::string(row[3]));

	// muted flag
	if (row[5])
		user->setIsMuted((int) row[5]);

	// clean up
	mysql_free_result(result);
}

void DBMySQL::getUserStatistics(const std::string &username, int &points, int &gamesPlayed, int &won, int &lost) throw(DBMySQL::Exception) {
	if (!m_Handle)
		throw DBMySQL::Exception("There is no current connection.");

	// form the sql string
	std::string sql="SELECT points, games_played, won, lost FROM statistics WHERE uid=(SELECT uid FROM users WHERE username='"+username+"')";

	// query the server
	if (mysql_query(m_Handle, sql.c_str()))
		throw DBMySQL::Exception("Unable to complete database query: "+std::string(mysql_error(m_Handle)));

	// get the results
	MYSQL_RES *result=mysql_store_result(m_Handle);
	MYSQL_ROW row=mysql_fetch_row(result);

	if (!row) {
		mysql_free_result(result);
		throw DBMySQL::Exception("The given user could not be found in the database.");
	}

	// now get the data stored in this row
	if (row[0]) points=atoi(row[0]);
	if (row[1]) gamesPlayed=atoi(row[1]);
	if (row[2]) won=atoi(row[2]);
	if (row[3]) lost=atoi(row[3]);

	// we're done
	mysql_free_result(result);
}

void DBMySQL::getUserProfile(const std::string &username, std::string &name, std::string &email, int &age, std::string &bio) throw(DBMySQL::Exception) {
	if (!m_Handle)
		throw DBMySQL::Exception("There is no current connection.");

	// form the sql string
	std::string sql="SELECT real_name, email, age, bio FROM users WHERE username='"+username+"'";

	// query the server
	if (mysql_query(m_Handle, sql.c_str()))
		throw DBMySQL::Exception("Unable to complete database query: "+std::string(mysql_error(m_Handle)));

	// get the results
	MYSQL_RES *result=mysql_store_result(m_Handle);
	MYSQL_ROW row=mysql_fetch_row(result);

	if (!row) {
		mysql_free_result(result);
		throw DBMySQL::Exception("The given user could not be found in the database.");
	}

	// now verify non-null data and save it
	if (row[0]) name=std::string(row[0]);
	if (row[1]) email=std::string(row[1]);
	if (row[2]) age=atoi(row[2]);
	if (row[3]) bio=std::string(row[3]);

	// free memory
	mysql_free_result(result);
}

void DBMySQL::updateUserProfile(const std::string &username, const std::string &name, const std::string &email, int &age, const std::string &bio) throw(DBMySQL::Exception) {
	if (!m_Handle)
		throw DBMySQL::Exception("There is no current connection.");

	// form the sql string
	std::stringstream ss;
	ss  << "UPDATE users SET real_name='" << name << "', email='" << email << "', age="
		<< age << ", bio='" << bio << "' WHERE username='" << username << "'";

	// query the server
	if (mysql_query(m_Handle, ss.str().c_str()))
		throw DBMySQL::Exception("Unable to complete database query: "+std::string(mysql_error(m_Handle)));
}

void DBMySQL::updateUserPassword(const std::string &username, const std::string &password) throw(DBMySQL::Exception) {
	if (!m_Handle)
		throw DBMySQL::Exception("There is no current connection.");

	// form the sql string
	std::string sql="UPDATE users SET password='"+password+"' WHERE username='"+username+"'";

	// query the server
	if (mysql_query(m_Handle, sql.c_str()))
		throw DBMySQL::Exception("Unable to complete database query: "+std::string(mysql_error(m_Handle)));
}

void DBMySQL::getUserList(const std::string &username, std::vector<std::string> &friends, bool blocked) throw(DBMySQL::Exception) {
	if (!m_Handle)
		throw DBMySQL::Exception("There is no current connection.");

	std::string sql="SELECT username FROM (SELECT other_id AS uid FROM userlists WHERE uid=(SELECT uid FROM users WHERE username='"+username+"') AND blocked="+(blocked ? "1" : "0")+") AS T NATURAL JOIN users";

	if (mysql_query(m_Handle, sql.c_str()))
		throw DBMySQL::Exception("Unable to complete database query: "+std::string(mysql_error(m_Handle)));

	// get the results
	MYSQL_RES *result=mysql_store_result(m_Handle);
	MYSQL_ROW row;

	while((row=mysql_fetch_row(result)))
		friends.push_back(std::string(row[0]));

	mysql_free_result(result);
}

void DBMySQL::updateUserList(const std::string &username, const std::vector<std::string> &list, bool blocked) throw(DBMySQL::Exception) {
	if (!m_Handle)
		throw DBMySQL::Exception("There is no current connection.");

	// first find this user's uid and cache it
	std::string sql="SELECT uid FROM users WHERE username='"+username+"'";
	if (mysql_query(m_Handle, sql.c_str()))
		throw DBMySQL::Exception("Unable to complete database query: "+std::string(mysql_error(m_Handle)));

	// get the results of this query
	MYSQL_RES *result=mysql_store_result(m_Handle);
	MYSQL_ROW row=mysql_fetch_row(result);
	if (!row)
		throw DBMySQL::Exception("Unable to find user's uid in database.");

	// cache the uid
	std::string uid=std::string(row[0]);
	mysql_free_result(result);

	// next purge all friend list entries for this user
	sql="DELETE FROM userlists WHERE uid="+uid+" AND blocked="+(blocked ? "1" : "0");
	if (mysql_query(m_Handle, sql.c_str()))
		throw DBMySQL::Exception("Unable to complete database query: "+std::string(mysql_error(m_Handle)));

	// now insert the updated records
	for (int i=0; i<list.size(); i++) {
		// find this user's uid
		sql="SELECT uid FROM users WHERE username='"+list[i]+"'";
		if (mysql_query(m_Handle, sql.c_str()))
			throw DBMySQL::Exception("Unable to complete database query: "+std::string(mysql_error(m_Handle)));

		result=mysql_store_result(m_Handle);
		row=mysql_fetch_row(result);
		if (row) {
			std::string otherUid=std::string(row[0]);
			mysql_free_result(result);

			// add this user list entry
			sql="INSERT INTO userlists VALUES("+uid+", "+otherUid+", NULL, "+(blocked ? "1" : "0")+")";
			if (mysql_query(m_Handle, sql.c_str()))
				throw DBMySQL::Exception("Unable to complete database query: "+std::string(mysql_error(m_Handle)));
		}
	}
}

DBMySQL::RequestResult DBMySQL::addUserToList(const std::string &username, const std::string &other, bool blocked) {
	if (!m_Handle)
		throw DBMySQL::Exception("There is no current connection.");

	// form the sql string
	std::string sql="INSERT INTO userlists VALUES((SELECT uid FROM users WHERE username='"+username+"'),";
	sql+=" (SELECT uid FROM users WHERE username='"+other+"'), NOW(), "+(blocked ? "1" : "0")+")";

	// query the server
	if (mysql_query(m_Handle, sql.c_str())) {
		// determine what when wrong
		std::string error=std::string(mysql_error(m_Handle));
		if (error.find("Duplicate")!=-1)
			return DBMySQL::DuplicateEntry;
		else
			throw DBMySQL::Exception("Unable to complete database query: "+error);
	}

	return DBMySQL::NoError;
}
