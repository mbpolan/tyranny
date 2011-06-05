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

/// Authentication class packets
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

#endif
