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
// packet.h: definitions of Packet class

#ifndef PACKET_H
#define PACKET_H

#include <iostream>
#include <stdint.h>

#define PACKET_BUFFER_MAX	1024
#define PACKET_STRING_MAX	PACKET_BUFFER_MAX-2

/**
 * Class that encapsulates raw network packets.
 * Normally, communication between server and client involves a series of
 * packets describing the data transaction. Instead of manually packing
 * a buffer of bytes to send, this class offers a simple and intuitive API
 * for automatically adding bytes, strings, and other data types.
 *
 * Packets consist of a 1 byte header followed by n bytes of data. The header is
 * guaranteed to be the third byte if a packet is properly packed. The very first two
 * bytes are reserved for calculating the packet size, and therefore, the first call to byte()
 * will return the header.
 */
class Packet {
	public:
		/// The result of a write or read operation.
		enum Result { NoError=0, DataCorrupt, Disconnected, TimedOut };

		/// Constructs an empty packet.
		Packet();
		
		/**
		 * Moves the insertion point of the packet n bytes back.
		 * It is important to note that rewinding past the third byte will cause
		 * the insertion point to be set to the third byte.
		 *
		 * @param bytes The amount of bytes to rewind by.
		 */
		void rewind(int bytes) { m_Pos-=bytes; }

		/**
		 * Returns the actual data size of the packet, excluding the first two bytes.
		 */
		int size() const { return m_Size; }

		/**
		 * Checks if a packet is empty.
		 */
		bool empty() const { return m_Size==0; }

		/**
		 * Resets the packet to an empty status.
		 */
		void clear();
		
		/**
		 * Adds a single byte to the packet.
		 *
		 * @param byte The byte to add.
		 */
		void addByte(uint8_t byte);

		/**
		 * Adds a 2 byte (short) unsigned interger to the packet.
		 *
		 * @param n The integer to add.
		 */
		void addUint16(uint16_t n);

		/**
		 * Adds a 4 byte (long) unsigned integer to the packet.
		 *
		 * @param n The integer to add.
		 */
		void addUint32(uint32_t n);

		/**
		 * Adds a variable length string to the packet.
		 * Note that the string cannot be longer than PACKET_STRING_MAX bytes.
		 *
		 * @param str The string to add.
		 */
		void addString(const std::string &str);
	
		/**
		 * Returns the current byte under the read position, without incrementing
		 * the read position.
		 *
		 * @return The byte at the current read position.
		 */
		uint8_t peekByte() { return m_Buffer[m_Pos]; }

		/**
		 * Returns a byte from the packet.
		 */
		uint8_t byte();

		/**
		 * Returns a 2 byte (short) unsigned integer from the packet.
		 */
		uint16_t uint16();

		/**
		 * Returns a 4 byte (long) unsigned integer from the packet.
		 */
		uint32_t uint32();

		/**
		 * Returns a variable length string from the packet.
		 */
		std::string string();
		
		/**
		 * Writes the packet's internal buffer to the given socket file descriptor.
		 * This method also handles partial sends, and so it is guaranteed that all
		 * the data will be sent out if no error occurs. This method will block until
		 * all data has been written.
		 *
		 * @param sock The socket to write to.
		 * @return True if the write succeeded, false otherwise.
		 */
		bool write(int sock);

		/**
		 * Reads a packet from the given socket file descriptor.
		 * Only one packet will be read at a time, and subsequent packets will require
		 * more calls to this function to read. Most importantly, this method blocks until
		 * data has been read or a time out has been reached. Do note that a
		 * time out return code does not mean that the peer has closed the connection; it
		 * simply means that no data has been received in the given time interval. A return
		 * code of Disconnected or DataCorrupt should be handled appropriately.
		 *
		 * @param socket The socket to read from.
		 * @return A result code.
		 */
		Result read(int socket);
	
	private:
		/// The internal packet buffer of bytes
		uint8_t m_Buffer[PACKET_BUFFER_MAX];

		/// Describes the actual packet size.
		short m_Size;

		/// Insertion point into the buffer.
		int m_Pos;
};

#endif
