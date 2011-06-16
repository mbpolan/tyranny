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
// lockable.h: definition and implementation of the Lockable interface class.

#ifndef LOCKABLE_H
#define LOCKABLE_H

#include <pthread.h>

/*
 * An interface class for objects that are thread-safe.
 * Any class that inherits Lockable is deemed "thread-safe," because
 * of the fact that this class provides an internal mutex that can be
 * locked or unlocked. Derived classes still have the responsibility
 * of carefully managing their locks, as deadlocks are easy to create
 * and very difficult to track down.
 */
class Lockable {
	public:
		/// Initializes the mutex lock.
		Lockable() {
			pthread_mutex_init(&m_Mutex, NULL);
		}

		/// Frees memory associated with this mutex.
		~Lockable() {
			pthread_mutex_destroy(&m_Mutex);
		}

		/// Locks the mutex.
		void lock() { pthread_mutex_lock(&m_Mutex); }

		/// Unlocks the mutex.
		void unlock() { pthread_mutex_unlock(&m_Mutex); }

	private:
		pthread_mutex_t m_Mutex;
};

#endif
