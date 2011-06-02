/***************************************************************************
 *   Copyright (C) 2010 by Michal Polan                                    *
 *   kanadakid@gmail.com                                                   *
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
// packet.h: definition of the Packet class

#ifndef PACKET_H
#define PACKET_H

#include <QObject>
#include <QTcpSocket>
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
class Packet: public QObject {
	Q_OBJECT
	
	public:
		/**
		 * Default constructor.
		 * Creates an empty packet.
		 */
		Packet();
		
		/**
		 * Clears the contents of the packet and resets it.
		 */
		void clear();
		
		/**
		 * Adds a single byte to the packet.
		 *
		 * @param n A byte to add.
		 */
		void addByte(uint8_t n);

		/**
		 * Adds a 16-bit unsigned integer to the packet.
		 *
		 * @param n A 16-bit integer to add.
		 */
		void addUint16(uint16_t n);

		/**
		 * Adds a 32-bit unsigned integer to the packet.
		 *
		 * @param n A 32-bit integer to add.
		 */
		void addUint32(uint32_t n);

		/**
		 * Adds a variable-length string to the packet.
		 *
		 * @param str The string to add.
		 */
		void addString(const QString &str);
		
		/**
		 * Returns the next byte in the packet.
		 *
		 * @return The next byte.
		 */
		uint8_t byte();

		/**
		 * Returns the next 16-bit unsigned integer in the packet.
		 *
		 * @return The next 16-bit integer.
		 */
		uint16_t uint16();

		/**
		 * Returns the next 32-bit unsigned integer in the packet.
		 *
		 * @return The next 32-bit integer.
		 */
		uint32_t uint32();

		/**
		 * Returns the next variable length string in the packet.
		 *
		 * @return The next string.
		 */
		QString string();
		
		/**
		 * Reads data waiting in the given socket buffer.
		 * Verify that the socket indeed has pending data beforehand, otherwise
		 * the results may be unexpected.
		 *
		 * @param sock The socket to read from.
		 * @return True if no error occurred, false otherwise.
		 */
		bool read(QTcpSocket *sock);

		/**
		 * Writes data pending in this packet buffer to the socket.
		 *
		 * @param sock The socket to write to.
		 * @return True if no error occurred, false otherwise.
		 */
		bool write(QTcpSocket *sock);
		
	private:
		/// Buffer which stores the packet data.
		uint8_t m_Buffer[PACKET_BUFFER_MAX];

		/// The size of the data buffer.
		int m_Size;

		/// Position to read/write from/to in the buffer.
		int m_Pos;
};		

#endif
