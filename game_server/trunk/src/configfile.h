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
 * Parser and cache for XML game server configuration files.
 * This class is responsible for parsing and verifying configuration data for the
 * game server.
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
		 * Returns id string for this server.
		 * @return The server id string.
		 */
		std::string getID() const { return m_ID; }

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
		 * Returns the IP address of the lobby server.
		 * @return The IP address/hostname of the lobby server.
		 */
		std::string getLobbyServerIP() const { return m_LobbyServerIP; }

		/**
		 * Returns the port number of the lobby server.
		 * @return Port number of the lobby server.
		 */
		int getLobbyServerPort() const { return m_LobbyServerPort; }

		/**
		 * Returns the path to the content package.
		 * @return Path to the content package.
		 */
		 std::string getContentPackagePath() const { return m_ContentPkg; }

	private:
		/**
		 * Parses the lobby-server XML section.
		 *
		 * @param node The root node of that section.
		 */
		 void parseLobbyServerData(void *node) throw(ConfigFile::Exception);

		/// The path of the configuration file to load.
		std::string m_Path;

		/// The server id string.
		std::string m_ID;

		/// IP address to bind to.
		std::string m_IP;

		/// Port number to bind to.
		int m_Port;
		
		/// The lobby server IP address.
		std::string m_LobbyServerIP;

		/// The lobby server port.
		int m_LobbyServerPort;

		/// Path to the content package.
		std::string m_ContentPkg;
};

#endif
