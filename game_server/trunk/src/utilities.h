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
// utilities.h: various helper functions

#ifndef UTILITIES_H
#define UTILITIES_H

#include <vector>

namespace Util {

/**
 * Draws a random positive number (32-bit integer) within the given range.
 *
 * @param low The lower bound.
 * @param high The upper bound.
 */
int randomInt(int low, int high);

/**
 * Generates a random turn order for four players.
 *
 * @return A vector of player indices, in turn order.
 */
std::vector<int> generateTurnOrder();

}

#endif
