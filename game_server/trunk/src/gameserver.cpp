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
 // gameserver.cpp: main entry point into the program.
 
 #include <cstdlib>
 #include <iostream>
 
 #include "configfile.h"
 #include "gameserver.h"
 #include "serversocket.h"
 
 // globals
 ConfigFile *g_ConfigFile=NULL;
 
 void* connectionHandler(void *arg) {
 	 ServerSocket::Client *data=(ServerSocket::Client*) arg;
 	 
 	 delete data;
 	 pthread_exit(0);
 }
 
 int main(int argc, char *argv[]) {
 	 // parse the config file
 	 g_ConfigFile=new ConfigFile("config.xml");
 	 
 	 std::cout << "Loading configuration file...\t";
 	 
 	 try {
 	 	g_ConfigFile->parse();
 	 }
 	 
 	 catch (const ConfigFile::Exception &ex) {
 	 	std::cout << "[fail]\n";
 	 	std::cout << ex.getMessage() << std::endl;
 	 	
 	 	exit(1);
 	 }
 	 
 	 std::cout << "[done]\n";
 	 
 	 // create the server socket
 	 ServerSocket sock(g_ConfigFile->getIP(), g_ConfigFile->getPort());
 	 
 	 std::cout << "Creating server socket...\t";
 	 try {
 	 	sock.bind();
 	 	sock.listen();
 	 }
 	 
 	 catch (const ServerSocket::Exception &ex) {
 	 	std::cout << "[fail]\n";
 	 	std::cout << ex.getMessage() << std::endl;
 	 	 
 	 	exit(1);
 	 }
 	 
 	 std::cout << "[done]\n";
 	 
 	 // print out some status messages
 	 std::cout << "Tyranny Game Server " << GAME_SERVER_VERSION << " running...\n";
 	 
 	 while(1) {
 	 	 ServerSocket::Client *cl=sock.accept();
 	 	 
 	 	 // create a new thread for this connection
 	 	 if (cl) {
 	 	 	 pthread_t th;
 	 	 	 pthread_create(&th, NULL, &connectionHandler, cl);
 	 	 }
 	 }
 	 
 	 return 0;
 }
 