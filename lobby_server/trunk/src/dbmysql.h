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

#include <memory>
#include <iostream>
#include <vector>
#include <mysql/mysql.h>

#include "user.h"

class DBMySQL;

typedef std::auto_ptr<DBMySQL> pDBMySQL;

class DBMySQL {
	public:
		/// Determines if a user is either playing in a room, the owner of a room, or neither.
		enum UserActivity { RoomOwner, Participant, NotActive };

		/// Property redistribution method.
		enum RedistMethod { RandomToPlayers=0, ReturnToBank };

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
		/// Results encountered when a user tries to add someone to a list.
		enum RequestResult { NoError, DuplicateEntry, UnknownUser };

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
		 * Caches connection and login information to quickly create new database handles.
		 *
		 * @param host The database server.
		 * @param port The database port.
		 * @param db The database name.
		 * @param user The database account username.
		 * @param password The database account password.
		 */
		static void cache(const std::string &host, int port, const std::string &db, const std::string &user, const std::string &password);

		/**
		 * Creates a new handle using cache data.
		 * Make sure to call cache() beforehand, otherwise this method will throw an exception.
		 *
		 * @return A ready-to-use database handle.
		 */
		static pDBMySQL synthesize() throw(DBMySQL::Exception);

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
		 * @throws DBMySQL::Exception An exception describing a disconnect problem.
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

		/**
		 * Flags a user as either online or offline.
		 *
		 * @param username The user to flag.
		 * @param online True for online, false for offline.
		 */
		void flagUserOnline(const std::string &username, bool online) throw(DBMySQL::Exception);

		/**
		 * Returns the user's game statistics as recorded in the database.
		 *
		 * @param username The user for whom to get statistics.
		 * @param points The points for the user.
		 * @param gamesPlayed The amount of games the user played.
		 * @param won The amount of games the user won.
		 * @param lost The amount of games the user lost.
		 */
		void getUserStatistics(const std::string &username, int &points, int &gamesPlayed, int &won, int &lost) throw(DBMySQL::Exception);

		/**
		 * Returns the user's profile data as recorded in the database.
		 *
		 * @param username The user for whom to get a profile.
		 * @param name The user's real name.
		 * @param email The user's email address.
		 * @param age The user's age.
		 * @param bio The user's biography.
		 */
		void getUserProfile(const std::string &username, std::string &name, std::string &email, int &age, std::string &bio) throw(DBMySQL::Exception);

		/**
		 * Updates the user's profile data and stores it in the database.
		 *
		 * @param username The user for whom to update the profile.
		 * @param name The user's real name.
		 * @param email The user's email address.
		 * @param age The user's age.
		 * @param bio The user's biography.
		 */
		void updateUserProfile(const std::string &username, const std::string &name, const std::string &email, int &age, const std::string &bio) throw(DBMySQL::Exception);

		/**
		 * Updates a user's password with a new one.
		 *
		 * @param username The user whose password should be changed.
		 * @param password The new password for the user.
		 */
		void updateUserPassword(const std::string &username, const std::string &password) throw(DBMySQL::Exception);

		/**
		 * Returns a user's friend list or blocked user list.
		 *
		 * @param username The user in question.
		 * @param list A vector of usernames.
		 * @param blocked True for blocked list, false for friends list.
		 */
		void getUserList(const std::string &username, std::vector<std::string> &list, bool blocked) throw(DBMySQL::Exception);

		/**
		 * Updates either a user's friend list or blocked user list.
		 *
		 * @param username The user for whom to update a friend list.
		 * @param list The list of usernames.
		 * @param blocked True to update blocked list, false for friends list.
		 */
		void updateUserList(const std::string &username, const std::vector<std::string> &list, bool blocked) throw(DBMySQL::Exception);

		/**
		 * Adds another user to the friends/blocked list of this user.
		 *
		 * @param username The user whose list should be modified.
		 * @param other The username of the user to add.
		 * @param blocked True to add to the blocked list, false for friends list.
		 * @return An code describing the results of the add request.
		 */
		RequestResult addUserToList(const std::string &username, const std::string &other, bool blocked) throw(DBMySQL::Exception);

		/**
		 * Determines if a user has already started a game room, or is playing in another game room.
		 *
		 * @param username The user to test.
		 * @return A UserActivity result code.
		 */
		UserActivity isUserActive(const std::string &username) throw(DBMySQL::Exception);

		/**
		 * Creates a new record for a game room started by the given user.
		 *
		 * @param owner The username of the room owner.
		 * @param maxTurns The maximum amount of game turns.
		 * @param maxHumans The maximum number of human players.
		 * @param freeParkingReward Money paid to the player by stepping on Free Parking.
		 * @param redistMethod Property redistribution method following player bankruptcy.
		 * @param incomeTaxChoice Whether or not players can choose the fee to pay on Income Tax.
		 * @param password The room password.
		 * @param onlyFriends Whether only friends of the owner may join the room.
		 * @return The assigned id number of the room.
		 */
		int createGameRoom(const std::string &owner, int maxTurns, int maxHumans, int freeParkingReward,
							const RedistMethod &rmethod, bool incomeTaxChoice, const std::string &password, bool onlyFriends) throw(DBMySQL::Exception);

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
