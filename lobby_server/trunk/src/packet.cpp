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
// packet.cpp: implementation of Packet class

#include <cerrno>
#include <sys/socket.h>

#include "packet.h"

Packet::Packet() {
	clear();
}

void Packet::clear() {
	m_Pos=2;
	m_Size=0;
	
}

void Packet::addByte(uint8_t byte) {
	m_Buffer[m_Pos++]=byte;
	m_Size+=1;
}

void Packet::addUint16(uint16_t n) {
	// pack a 16-bit integer into the packet
	m_Buffer[m_Pos++]=(uint8_t) n;
	m_Buffer[m_Pos++]=(uint8_t) (n >> 8);
	m_Size+=2;
}

void Packet::addUint32(uint32_t n) {
	// pack a 32-bit integer into the buffer
	m_Buffer[m_Pos++]=(uint8_t) n;
	m_Buffer[m_Pos++]=(uint8_t) (n >> 8);
	m_Buffer[m_Pos++]=(uint8_t) (n >> 16);
	m_Buffer[m_Pos++]=(uint8_t) (n >> 24);
	m_Size+=4;
	
}

void Packet::addString(const std::string &str) {
	if (str.size()>PACKET_STRING_MAX)
		return;
	
	// add the string size first
	addUint16(str.size());
	for (int i=0; i<str.size(); i++)
		addByte(str[i]);
}

uint8_t Packet::byte() {
	return m_Buffer[m_Pos++];
}

uint16_t Packet::uint16() {
	// unpack a 16-bit integer from the buffer
	uint16_t n=(m_Buffer[m_Pos] | m_Buffer[m_Pos+1] << 8);
	m_Pos+=2;
	
	return n;
}

uint32_t Packet::uint32() {
	// unpack a 32-bit integer from the buffer
	uint32_t n=(m_Buffer[m_Pos] | (m_Buffer[m_Pos+1] << 8) | 
				(m_Buffer[m_Pos+2] << 16) | (m_Buffer[m_Pos+3] << 24));
	m_Pos+=4;
	
	return n;
}

std::string Packet::string() {
	uint16_t length=uint16();
	
	// avoid crashing if the length is corrupt
	if (length>PACKET_STRING_MAX)
		return "";
	
	std::string str="";
	for (int i=0; i<length; i++)
		str+=(char) byte();
	
	return str;
}

bool Packet::write(int fd) {
	// save the packet size to buffer
	m_Buffer[0]=m_Size;
	m_Buffer[1]=(m_Size >> 8);
	
	int sent=0, remaining=m_Size+2;
	int n;
	while(sent<m_Size) {
		if ((n=send(fd, m_Buffer+sent, remaining, 0))==-1)
			return false;
		
		sent+=n;
		remaining-=n;
	}

	return (n!=-1);
}

Packet::Result Packet::read(int fd) {
	// read the size first
	int n=recv(fd, m_Buffer, 2, 0);
	if (n==-1 && errno==EWOULDBLOCK)
		return TimedOut;
	else if (n==0)
		return Disconnected;
	
	int size=(m_Buffer[0] | (m_Buffer[1] >> 8));
	int bytes=recv(fd, m_Buffer+2, size, 0);
	
	if (bytes!=size || bytes<=0) {
		std::cout << "read " << bytes << " was expecting " << size << std::endl;
		return DataCorrupt;
	}
	
	m_Size=size;
	m_Pos=2;
	
	return NoError;
}
