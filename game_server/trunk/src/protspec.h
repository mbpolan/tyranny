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
// protspec.h: definition of the game server protocol.

#ifndef PROTSPEC_H
#define PROTSPEC_H

/// Types of incoming connections.
#define CONN_CLIENT		0x00
#define CONN_LOBBY		0x01
#define CONN_GAME		0x02

/// Inter-server communication.
#define IS_OPENROOM		0x00
#define IS_KILLROOM		0x01

/// Room parameters.
#define PROP_RANDOM			0x00	// property distributed randomly to players
#define PROP_RETURNBANK		0x01	// property returned to bank

/*************************************************************************/

/// Room controls.
#define GMRM_START_WAIT		0xC0
#define GMRM_BEGIN_GAME		0xC1

#endif
