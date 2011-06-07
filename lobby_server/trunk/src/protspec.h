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
// protspec.h: definition of the client-server protocol.

#ifndef PROTSPEC_H
#define PROTSPEC_H

/// User statuses.
#define USER_NONE			0x00
#define USER_BLOCKED		0x01
#define USER_FRIEND			0x02

/// Types of user requests and results.
#define REQ_FRIENDS			0x01
#define REQ_BLOCKED			0x02
#define REQ_SUCCESS			0x03
#define REQ_ERROR			0x04

/// Authentication and login class packets
#define AUTH_DATA			0xA0
#define AUTH_SUCCESS		0xA1
#define AUTH_ERROR			0xA2
#define AUTH_LOGOUT			0xA3
#define AUTH_REQUEST		0xA4

/// General lobby actions
#define LB_USERIN			0xB1
#define LB_USEROUT			0xB2
#define LB_CHATMESSAGE		0xB3
#define LB_STATISTICS		0xB4
#define LB_USERPROFILE_REQ	0xB5
#define LB_USERPROFILE_UPD	0xB6
#define LB_CHANGEPASSWORD	0xB7
#define LB_FRIENDS_REQ		0xB8
#define LB_FRIENDS_UPD		0xB9
#define LB_BLOCKED_REQ		0xBA
#define LB_BLOCKED_UPD		0xBB
#define LB_USERREQUEST		0xBC

/// Server messages
#define MSG_INFO			0xE1
#define MSG_ERROR			0xE2

#endif
