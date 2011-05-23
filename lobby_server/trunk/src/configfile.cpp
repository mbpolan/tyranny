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

ConfigFile::ConfigFile(const std::string &path) {
	m_Path=path;

	// set some defaults
	m_IP="";
	m_Port=0;
	m_MaxClients=0;
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
		// ip address
		if (xmlStrcmp(child->name, (const xmlChar*) "ip")==0)
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
