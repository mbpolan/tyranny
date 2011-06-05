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
// statsdialog.h: definition of the StatsDialog class.

#ifndef STATSDIALOG_H
#define STATSDIALOG_H

#include <QDialog>

namespace Ui {
	class StatsDialog;
}

/**
 * Simple dialog for displaying user statistics.
 * Once a set of statistics for a user has been received from the server, this
 * dialog formats that data and displays it in a readable way for the user to
 * review.
 */
class StatsDialog: public QDialog {
	Q_OBJECT

	public:
		/**
		 * Creates a dialog with the given statistics information.
		 *
		 * @param points The amount of points the played accured.
		 * @param gamesPlayed The amount of games the player as played.
		 * @param won The amount of games the player won.
		 * @param lost The amount of games the player lost.
		 * @param parent The parent window for this dialog.
		 */
		StatsDialog(int points, int gamesPlayed, int won, int lost, QWidget *parent=NULL);

	private:
		Ui::StatsDialog *ui;
};

#endif
