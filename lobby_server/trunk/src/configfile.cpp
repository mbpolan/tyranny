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
// configfile.cpp: implementation of the ConfigFile class.

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "configfile.h"

// global configuration file
ConfigFile *g_CfgFile=NULL;

ConfigFile::ConfigFile(const std::string &path) {
	m_Path=path;

	// set some defaults
	m_Name="Tyranny Lobby Server";
	m_IP="";
	m_Port=0;
	m_MaxClients=0;

	g_CfgFile=this;
}

ConfigFile* ConfigFile::instance() {
	return g_CfgFile;
}

void ConfigFile::parse() throw(ConfigFile::Exception) {
	xmlDocPtr doc;
	xmlNodePtr root;

	// try to parse the file
	if (!(doc=xmlParseFile(m_Path.c_str())))
		throw ConfigFile::Exception("Unable to open configuration file.");

	// grab the root element
	if (!(root=xmlDocGetRootElement(doc))) {
		xmlFreeDoc(doc);
		throw ConfigFile::Exception("Missing root element in configuration file.");
	}

	// start walking the document
	xmlNodePtr child=root->children;
	while(child) {
		// server name
		if (xmlStrcmp(child->name, (const xmlChar*) "name")==0)
			m_Name=std::string((const char*) xmlNodeGetContent(child));

		// ip address
		else if (xmlStrcmp(child->name, (const xmlChar*) "ip")==0)
			m_IP=std::string((const char*) xmlNodeGetContent(child));

		// port
		else if (xmlStrcmp(child->name, (const xmlChar*) "port")==0) {
			const char *pval=(const char*) xmlNodeGetContent(child);
			m_Port=atoi(pval);
		}

		// max clients
		else if (xmlStrcmp(child->name, (const xmlChar*) "max-clients")==0) {
			const char *pval=(const char*) xmlNodeGetContent(child);
			m_MaxClients=atoi(pval);
		}

		// server list
		else if (xmlStrcmp(child->name, (const xmlChar*) "servers")==0) {
			try {
				parseServerList((void*) child);
			}
			catch (const ConfigFile::Exception &ex) {
				throw ex;
			}
		}

		// mysql connection data
		else if (xmlStrcmp(child->name, (const xmlChar*) "mysql")==0) {
			try {
				parseMySQLSection((void*) child);
			}
			catch (const ConfigFile::Exception &ex) {
				throw ex;
			}
		}

		child=child->next;
	}
}

void ConfigFile::parseServerList(void *node) throw(ConfigFile::Exception) {
	xmlNodePtr child=(xmlNodePtr) node;
	xmlNodePtr snode=child->children;
	while(snode) {
		if (xmlStrcmp(snode->name, (const xmlChar*) "server")!=0) {
			snode=snode->next;
			continue;
		}

		const char *id=NULL;
		const char *ip=NULL;
		int port=0;

		// get the server identifier
		id=(const char*) xmlGetProp(snode, (xmlChar*) "id");

		// start walking the subsections
		xmlNodePtr enode=snode->xmlChildrenNode;
		while(enode) {
			// ip address
			if (xmlStrcmp(enode->name, (const xmlChar*) "ip")==0) {
				ip=(const char*) xmlNodeGetContent(enode);
			}

			// port number
			else if (xmlStrcmp(enode->name, (const xmlChar*) "port")==0) {
				const char *pval=(const char*) xmlNodeGetContent(enode);
				port=atoi(pval);
			}

			enode=enode->next;
		}

		// verify that we have all the needed data
		if (!id)
			throw ConfigFile::Exception("Missing identifier for game server.");
		else if (!ip)
			throw ConfigFile::Exception("Missing IP address for game server.");
		else if (port==0)
			throw ConfigFile::Exception("Missing port number for game server.");

		// otherwise create an object for this server and add it to the list
		ConfigFile::Server server(std::string(id), std::string(ip), port);
		m_GameServers.push_back(server);

		snode=snode->next;
	}
}

void ConfigFile::parseMySQLSection(void *node) throw(ConfigFile::Exception) {
	xmlNodePtr child=(xmlNodePtr) node;
	xmlNodePtr snode=child->children;

	const char *host=NULL;
	const char *name=NULL;
	const char *user=NULL;
	const char *pass=NULL;
	int port=0;

	while(snode) {
		if (xmlStrcmp(snode->name, (const xmlChar*) "host")==0)
			host=(const char*) xmlNodeGetContent(snode);

		else if (xmlStrcmp(snode->name, (const xmlChar*) "name")==0)
			name=(const char*) xmlNodeGetContent(snode);

		else if (xmlStrcmp(snode->name, (const xmlChar*) "username")==0)
			user=(const char*) xmlNodeGetContent(snode);

		else if (xmlStrcmp(snode->name, (const xmlChar*) "password")==0)
			pass=(const char*) xmlNodeGetContent(snode);

		else if (xmlStrcmp(snode->name, (const xmlChar*) "port")==0)
			port=atoi((const char*) xmlNodeGetContent(snode));

		snode=snode->next;
	}

	// verify that we have all the needed data
	if (!host)
		throw ConfigFile::Exception("Missing host element for MySQL data.");
	if (!name)
		throw ConfigFile::Exception("Missing name element for MySQL data.");
	if (!user)
		throw ConfigFile::Exception("Missing username element for MySQL data.");
	if (!pass)
		throw ConfigFile::Exception("Missing password element for MySQL data.");
	if (port==0)
		throw ConfigFile::Exception("Missing port element for MySQL data.");

	// sync all the data
	m_DBHost=std::string(host);
	m_DBPort=port;
	m_DBName=std::string(name);
	m_DBUser=std::string(user);
	m_DBPassword=std::string(pass);
}
