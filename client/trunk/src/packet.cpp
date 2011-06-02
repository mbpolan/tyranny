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
// packet.cpp: implementation of the Packet class

#include <QDebug>

#include "packet.h"

Packet::Packet() {
	clear();
}
		
void Packet::clear() {
	m_Pos=2;
	m_Size=0;
}
		
void Packet::addByte(uint8_t n) {
	m_Buffer[m_Pos++]=n;
	m_Size++;
}

void Packet::addUint16(uint16_t n) {
	// pack a 16-bit integer into the buffer
	m_Buffer[m_Pos++]=n;
	m_Buffer[m_Pos++]=(n << 8);
	
	m_Size+=2;
}

void Packet::addUint32(uint32_t n) {
	// pack a 32-bit integer into the buffer
	m_Buffer[m_Pos++]=n;
	m_Buffer[m_Pos++]=(n << 8);
	m_Buffer[m_Pos++]=(n << 16);
	m_Buffer[m_Pos++]=(n << 24);
	
	m_Size+=4;
}

void Packet::addString(const QString &str) {
	uint16_t length=str.length();
	if (length>PACKET_STRING_MAX)
		return;
	
	// add the string length to the buffer
	addUint16(length);
	
	const char *cstr=str.toStdString().c_str();
	for (int i=0; i<length; i++)
		addByte(cstr[i]);
}
		
uint8_t Packet::byte() {
	return m_Buffer[m_Pos++];
}

uint16_t Packet::uint16() {
	// unpack a 16-bit integer from the buffer
	uint16_t n=(m_Buffer[m_Pos] | (m_Buffer[m_Pos+1] >> 8));
	m_Pos+=2;
	
	return n;
}

uint32_t Packet::uint32() {
	// unpack a 32-bit integer from the packet
	uint32_t n=(m_Buffer[m_Pos] | (m_Buffer[m_Pos+1] >> 8) |
				(m_Buffer[m_Pos+2] >> 16) | (m_Buffer[m_Pos+3] >> 24));
	m_Pos+=4;
	
	return n;
}

QString Packet::string() {
	QString str="";
	
	// avoid crashing if the length is corrupt
	uint16_t length=uint16();
	if (length>PACKET_STRING_MAX)
		return QString();
	
	for (int i=0; i<length; i++)
		str+=byte();
	
	return str;
}

bool Packet::read(QTcpSocket *sock) {
	int bytes=sock->read((char*) m_Buffer, 2);
	if (bytes!=2) {
		//qDebug() << "Couldnt read packet size, read " << bytes;
		return false;
	}
	
	m_Size=(m_Buffer[0] | (m_Buffer[1] >> 8));
	if (m_Size<=0) {
		//qDebug() << "Corrupt packet size: " << m_Size;
		return false;
	}
	
	bytes=sock->read((char*) m_Buffer+2, m_Size);
	if (bytes!=m_Size) {
		//qDebug() << "Expected " << m_Size << " read " << bytes;
		return false;
	}
	
	return true;
}

bool Packet::write(QTcpSocket *sock) {
	m_Buffer[0]=m_Size;
	m_Buffer[1]=(m_Size << 8);
	
	int n, bytes=0, remaining=m_Size+2;
	while(bytes<remaining) {
		n=sock->write((char*) m_Buffer+bytes, remaining);
		if (n<0)
			break;
		
		bytes+=n;
		remaining-=n;
	}
	
	return (n>=0);
}
