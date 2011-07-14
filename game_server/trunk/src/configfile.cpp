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
	m_ID="Tyranny Game Server 1";
	m_IP="";
	m_Port=0;
	m_LobbyServerIP="";
	m_LobbyServerPort=0;
	m_ContentPkg="";

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
		// server id
		if (xmlStrcmp(child->name, (const xmlChar*) "id")==0)
			m_ID=std::string((const char*) xmlNodeGetContent(child));

		// ip address
		else if (xmlStrcmp(child->name, (const xmlChar*) "ip")==0)
			m_IP=std::string((const char*) xmlNodeGetContent(child));

		// port
		else if (xmlStrcmp(child->name, (const xmlChar*) "port")==0) {
			const char *pval=(const char*) xmlNodeGetContent(child);
			m_Port=atoi(pval);
		}

		// content package
		else if (xmlStrcmp(child->name, (const xmlChar*) "content-pkg")==0)
			m_ContentPkg=std::string((const char*) xmlNodeGetContent(child));

		// lobby server data
		else if (xmlStrcmp(child->name, (const xmlChar*) "lobby-server")==0) {
			try {
				parseLobbyServerData(child);
			}
			catch (const ConfigFile::Exception &ex) {
				throw ex;
			}
		}

		child=child->next;
	}
}

void ConfigFile::parseLobbyServerData(void *node) throw(ConfigFile::Exception) {
	xmlNodePtr n=(xmlNodePtr) node;
	xmlNodePtr ptr=n->children;

	const char *ip=NULL;
	int port=0;

	while(ptr) {
		if (xmlStrcmp(ptr->name, (const xmlChar*) "ip")==0)
			ip=(const char*) xmlNodeGetContent(ptr);

		else if (xmlStrcmp(ptr->name, (const xmlChar*) "port")==0) {
			const char *pval=(const char*) xmlNodeGetContent(ptr);
			port=atoi(pval);
		}

		ptr=ptr->next;
	}

	// verify we got all the data
	if (!ip)
		throw ConfigFile::Exception("Missing lobby server IP.");
	if (port==0)
		throw ConfigFile::Exception("Missing lobby server port.");

	m_LobbyServerIP=std::string(ip);
	m_LobbyServerPort=port;
}
