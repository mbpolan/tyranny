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
// fdbuffer.h: definition of the FDBuffer class.

#ifndef FDBUFFER_H
#define FDBUFFER_H

#include <sys/types.h>
#include <sys/time.h>
#include <vector>

class FDBuffer {
	public:
		/// Reasons the wait() function returns.
		enum WaitCode { NoAction, TimeExpired, DataReady };

	public:
		/// Creates an empty socket buffer.
		FDBuffer();

		/**
		 * Adds a socket file descriptor to the internal set.
		 *
		 * @param fd The socket to add.
		 */
		void addSocket(int fd);

		/**
		 * Removes a socket file descriptor from the internal set.
		 *
		 * @param fd The socket to remove.
		 */
		void removeSocket(int fd);

		/**
		 * Sets the amount of seconds before a timeout should be called.
		 *
		 * @param delta The amount of seconds.
		 */
		void setExpireTime(int seconds);

		/**
		 * Waits on the set of sockets, either for data or timeout.
		 * This method will do two important things:
		 *   1) Wait for ANY socket to receive data
		 *   2) Determine if the next timeout interval has been reached
		 *
		 * There are three cases to consider:
		 *   1) If NO socket has reported data and the next timeout has not been reached,
		 *      the NoAction enum will be returned.
		 *   2) If ANY socket has reported data, DataReady will be returned.
		 *   3) If the next timeout has been reached, TimeExpired will be returned.
		 */
		WaitCode poll();

		/**
		 * Interrupts an ongoing call to poll().
		 * This method sends a signal to the internal polling system, terminating its
		 * blocking state. This effectively causes poll() to return immediately.
		 */
		void awake();

		/**
		 * Returns a vector of sockets with data ready to be read.
		 * Note that this vector could be empty. Always call this method if the
		 * poll() method returns the DataReady code.
		 *
		 * @return A vector of socket file descriptors.
		 */
		std::vector<int> getActiveSockets() const { return m_ActiveFDs; }

	private:
		/// The collection of socket file descriptors.
		fd_set m_FDs;

		/// The next timeout event.
		struct timeval m_NextExpire;

		/// The highest file descriptor number.
		int m_MaxFD;

		/// Pipe for interrupting the blocking polling system.
		int m_ComPipe[2];

		/// Vector of sockets with data waiting.
		std::vector<int> m_ActiveFDs;
};

#endif
