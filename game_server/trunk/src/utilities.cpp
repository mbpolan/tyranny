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
// utilities.cpp: implementation of Util namespace functions.

#include <cstdlib>
#include <sys/socket.h>

#include "utilities.h"

int Util::randomInt(int low, int high) {
	return (low+int((high-low+1)*rand()/RAND_MAX+1.0));
}

std::vector<int> Util::generateTurnOrder() {
	// an array of possible turn orders
	int options[][4]={
			{ 0, 1, 2, 3 },
			{ 0, 2, 1, 3 },
			{ 0, 1, 3, 2 },
			{ 0, 3, 2, 1 },
			{ 0, 2, 3, 1 },
			{ 0, 3, 1, 2 },

			{ 1, 0, 2, 3 },
			{ 1, 0, 3, 2 },
			{ 1, 3, 0, 2 },
			{ 1, 2, 0, 3 },
			{ 1, 3, 2, 0 },
			{ 1, 2, 3, 0 },

			{ 2, 0, 1, 3 },
			{ 2, 0, 3, 1 },
			{ 2, 1, 0, 3 },
			{ 2, 1, 3, 0 },
			{ 2, 3, 0, 1 },
			{ 2, 3, 1, 0 },

			{ 3, 0, 1, 2 },
			{ 3, 0, 2, 1 },
			{ 3, 1, 0, 2 },
			{ 3, 1, 2, 0 },
			{ 3, 2, 0, 1 },
			{ 3, 2, 1, 2 }
	};

	// draw a random number and choose one of the arrays
	int draw=randomInt(0, 23);

	std::vector<int> orders=std::vector<int>(4);
	orders[0]=options[draw][0];
	orders[1]=options[draw][1];
	orders[2]=options[draw][2];
	orders[3]=options[draw][3];

	return orders;
}
