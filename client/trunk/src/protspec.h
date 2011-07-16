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

/// General codes.
#define PKT_ERROR			0x00
#define PKT_SUCCESS		0x01

/// Types of incoming connections.
#define CONN_CLIENT		0x00
#define CONN_LOBBY		0x01
#define CONN_GAME			0x02

/// User statuses.
#define USER_NONE			0x00
#define USER_BLOCKED		0x01
#define USER_FRIEND		0x02

/// Types of user requests.
#define REQ_FRIENDS		0x01
#define REQ_BLOCKED		0x02
#define REQ_SUCCESS		0x03
#define REQ_ERROR			0x04

/// Actions for changes to the list of rooms.
#define LB_ROOM_UPDATE		0x00
#define LB_ROOM_DELETE		0x01

/// Room parameters.
#define PROP_RANDOM		0x00	// property distributed randomly to players
#define PROP_RETURNBANK		0x01	// property returned to bank
#define ROOM_OPEN			0x02
#define ROOM_INPROGRESS		0x03
#define ROOM_CLOSED		0x04
#define ROOM_PUBLIC		0x05
#define ROOM_PRIVATE		0x06

/****************************************************************************/

/// Authentication class packets
#define AUTH_DATA			0xA0
#define AUTH_SUCCESS		0xA1
#define AUTH_ERROR		0xA2
#define AUTH_LOGOUT		0xA3
#define AUTH_REQUEST		0xA4

/// General lobby actions
#define LB_USERIN			0xB0
#define LB_USEROUT		0xB1
#define LB_CHATMESSAGE		0xB2
#define LB_STATISTICS		0xB3
#define LB_USERPROFILE_REQ	0xB4
#define LB_USERPROFILE_UPD	0xB5
#define LB_CHANGEPASSWORD	0xB6
#define LB_FRIENDS_REQ		0xB7
#define LB_FRIENDS_UPD		0xB8
#define LB_BLOCKED_REQ		0xB9
#define LB_BLOCKED_UPD		0xBA
#define LB_USERREQUEST		0xBB
#define LB_CREATEROOM		0xBC
#define LB_JOINROOM		0xBD
#define LB_ROOMLIST_REFRESH	0xBE
#define LB_ROOMLIST_UPD		0xBF

/// Server messages
#define MSG_INFO			0xE1
#define MSG_ERROR			0xE2

/// Room controls.
#define GMRM_START_WAIT		0xC0
#define GMRM_BEGIN_GAME		0xC1

/// Game related packets.
#define GAME_TURN_ORDER		0xD0
#define GAME_PLAYER_JOINED	0xD1
#define GAME_PLAYER_QUIT	0xD2
#define GAME_CHOOSE_TOK		0xD3
#define GAME_SELECTED_TOK	0xD4

#endif
