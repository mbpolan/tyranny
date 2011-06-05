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
// configfile.h: declaration of the ConfigFile class.

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <iostream>
#include <vector>

/**
 * Parser and cache for XML lobby server configuration files.
 * This class is responsible for parsing and verifying configuration data for the
 * lobby server. It also stores a list of game server that are associated with this
 * particular lobby server, as defined by the configuration file.
 */
class ConfigFile {
	public:
		/**
		 * A general exception for configuration files.
		 */
		class Exception {
			public:
				/**
				 * Default constructor with reason message.
				 */
				Exception(const std::string &msg) { m_Message=msg; }

				/**
				 * Returns the reason for this exception.
				 * @return A reason message.
				 */
				std::string getMessage() const { return m_Message; }

			private:
				/// The reason for this exception.
				std::string m_Message;
		};

		/**
		 * Data about an associated game server.
		 * These are defined in the configuration file by the <server> tags.
		 */
		class Server {
			public:
				/**
				 * Default constructor for setting data for this server object.
				 */
				Server(const std::string &id, const std::string &ip, int port): m_ID(id), m_IP(ip), m_Port(port) { };

				/**
				 * Returns the identifier string for this server.
				 * @return The server identifier.
				 */
				std::string getIDString() const { return m_ID; }

				/**
				 * Returns the IP address of the server.
				 * @return The server IP address.
				 */
				std::string getIP() const { return m_IP; }

				/**
				 * Returns the port number of the server.
				 * @return The server port number.
				 */
				int getPort() const { return m_Port; }

			private:
				/// The unique server identifier string.
				std::string m_ID;

				/// The IP address of the server.
				std::string m_IP;

				/// The port number of the server.
				int m_Port;
		};

	public:
		/**
		 * Default constructor.
		 * @param path The path to the XML configuration file to work on.
		 */
		ConfigFile(const std::string &path);

		/**
		 * Returns an instance to the global configuration file.
		 *
		 * @return A pointer to a ConfigFile object.
		 */
		static ConfigFile* instance();

		/**
		 * Parses the set configuration file and caches data.
		 * @throw A ConfigFile::Exception if an error occurs.
		 */
		void parse() throw(ConfigFile::Exception);

		/**
		 * Returns name for this server.
		 * @return The server name.
		 */
		std::string getName() const { return m_Name; }

		/**
		 * Returns the assigned IP address for the server.
		 * @return IP address to bind to.
		 */
		std::string getIP() const { return m_IP; }

		/**
		 * Returns the assigned port for the server.
		 * @return Port number to bind to.
		 */
		int getPort() const { return m_Port; }

		/**
		 * Returns the maximum client count.
		 * @return Maximum number of connections for this server.
		 */
		int getMaxClients() const { return m_MaxClients; }

		/**
		 * Returns a list of defined game servers.
		 * @return Vector of game servers associated with this login server.
		 */
		std::vector<ConfigFile::Server> getGameServerList() const { return m_GameServers; }

		/**
		 * Returns the database server hostname.
		 * @return The DBMS server hostname or IP address.
		 */
		std::string getDBHost() const { return m_DBHost; }

		/**
		 * Returns the database port number.
		 * @return The DBMS server port number.
		 */
		int getDBPort() const { return m_DBPort; }

		/**
		 * Returns the database name.
		 * @return The DBMS server data store name.
		 */
		std::string getDBName() const { return m_DBName; }

		/**
		 * Returns the database username.
		 * @return The DBMS server access username.
		 */
		std::string getDBUser() const { return m_DBUser; }

		/**
		 * Returns the database password.
		 * @return The DBMS server user password.
		 */
		std::string getDBPassword() const { return m_DBPassword; }

	private:
		/**
		 * Parses the list of associated game servers.
		 * @param node The root node of the <servers> ... </servers> elements
		 */
		void parseServerList(void *node) throw(ConfigFile::Exception);

		/**
		 * Parses the section containing MySQL connection information.
		 * @param node The root node of the <mysql> ... </mysql> elements
		 */
		void parseMySQLSection(void *node) throw(ConfigFile::Exception);

		/// The path of the configuration file to load.
		std::string m_Path;

		/// The server name.
		std::string m_Name;

		/// IP address to bind to.
		std::string m_IP;

		/// Port number to bind to.
		int m_Port;

		/// Maximum number of client connections allowed.
		int m_MaxClients;

		/// List of associated game servers.
		std::vector<ConfigFile::Server> m_GameServers;

		/// The database server hostname or IP address.
		std::string m_DBHost;

		/// The database server port number.
		int m_DBPort;

		/// The database data store name.
		std::string m_DBName;

		/// The database username.
		std::string m_DBUser;

		/// The database user password.
		std::string m_DBPassword;
};

#endif
