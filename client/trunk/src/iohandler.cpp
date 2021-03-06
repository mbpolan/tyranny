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
// iohandler.cpp: implementation of the IOHandler class.

#include <cstdio>
#include <QVector>

#include "iohandler.h"

IOHandler::IOHandler(const QString &path, QObject *parent): QObject(parent), m_Path(path) {
	m_File=NULL;
}

bool IOHandler::loadPreferences(QString *ip, int *port, QVector<QPair<QString, int> > *serverList) {
	m_File=fopen(m_Path.toStdString().c_str(), "r");
	if (!m_File)
		return false;

	// read the target ip
	int length;
	fread(&length, sizeof(int), 1, m_File);

	char buffer1[length+1];
	fread(buffer1, sizeof(char), length, m_File);
	buffer1[length]='\0';
	(*ip)=QString(buffer1);

	// read the port number
	fread(port, sizeof(int), 1, m_File);

	// read the amount of saved servers
	int amount;
	fread(&amount, sizeof(int), 1, m_File);

	// iterate over saved servers
	for (int i=0; i<amount; i++) {
		QString sIp;
		int sPort;

		fread(&length, sizeof(int), 1, m_File);

		char buffer2[length+1];
		fread(buffer2, sizeof(char), length, m_File);
		buffer2[length]='\0';
		sIp=QString(buffer2);

		fread(&sPort, sizeof(int), 1, m_File);

		(*serverList).push_back(QPair<QString, int>(sIp, sPort));
	}

	fclose(m_File);
	m_File=NULL;

	return true;
}

bool IOHandler::savePreferences(const QString &ip, int port, const QVector<QPair<QString, int> > &serverList) {
	m_File=fopen(m_Path.toStdString().c_str(), "w");
	if (!m_File)
		return false;

	// write the target ip
	int length=ip.length();
	fwrite(&length, sizeof(int), 1, m_File);
	fwrite(ip.toStdString().c_str(), sizeof(char), length, m_File);

	// write the port number
	fwrite(&port, sizeof(int), 1, m_File);

	// write the amount of saved servers
	int amount=serverList.size();
	fwrite(&amount, sizeof(int), 1, m_File);

	// iterate over saved servers
	for (int i=0; i<amount; i++) {
		QString sIp=serverList[i].first;
		int sPort=serverList[i].second;

		length=sIp.length();
		fwrite(&length, sizeof(int), 1, m_File);
		fwrite(sIp.toStdString().c_str(), sizeof(char), length, m_File);
		fwrite(&sPort, sizeof(int), 1, m_File);
	}

	fclose(m_File);
	m_File=NULL;

	return true;
}

