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
// fdbuffer.cpp: implementation of the FDBuffer class.

#include <cstdlib>
#include <fcntl.h>
#include <iostream>

#include "fdbuffer.h"

FDBuffer::FDBuffer() {
	FD_ZERO(&m_FDs);
	m_NextExpire.tv_sec=0;
	m_NextExpire.tv_usec=0;

	pipe(m_ComPipe);
	m_MaxFD=m_ComPipe[0]+1;

	// make the pipe non-blocking
	int flags=fcntl(m_ComPipe[0], F_GETFL);
	fcntl(m_ComPipe[0], F_SETFL, flags | O_NONBLOCK);

	flags=fcntl(m_ComPipe[1], F_GETFL);
	fcntl(m_ComPipe[1], F_SETFL, flags | O_NONBLOCK);

	FD_SET(m_ComPipe[0], &m_FDs);
}

void FDBuffer::addSocket(int fd) {
	FD_SET(fd, &m_FDs);

	if (fd>m_MaxFD)
		m_MaxFD=fd;
}

void FDBuffer::removeSocket(int fd) {
	FD_CLR(fd, &m_FDs);
}

void FDBuffer::setExpireTime(int delta) {
	gettimeofday(&m_NextExpire, NULL);
	m_NextExpire.tv_sec+=delta;
}

FDBuffer::WaitCode FDBuffer::poll() {
	// first see if we passed the designated timeout
	struct timeval tv, now;
	gettimeofday(&now, NULL);

	if (m_NextExpire.tv_sec>0 && now.tv_sec>m_NextExpire.tv_sec)
		return TimeExpired;

	fd_set tmp;
	FD_ZERO(&tmp);

	// set a timeout if necessary
	tmp=m_FDs;
	tv.tv_sec=m_NextExpire.tv_sec-now.tv_sec;
	tv.tv_usec=0;

	// wait for something to happen
	select(m_MaxFD+1, &tmp, NULL, NULL, (m_NextExpire.tv_sec>0 ? &tv : NULL));

	m_ActiveFDs.clear();
	for (int i=0; i<=m_MaxFD; i++) {
		if (FD_ISSET(i, &tmp)) {
			// signal sent through pipe
			if (i==m_ComPipe[0]) {
				char ch;
				while(read(m_ComPipe[0], &ch, 1)!=-1);
			}

			// a client socket has data or disconnected
			else
				m_ActiveFDs.push_back(i);
		}
	}

	if (m_ActiveFDs.empty())
		return NoAction;
	else
		return DataReady;
}

void FDBuffer::awake() {
	write(m_ComPipe[1], "1", 1);
}
