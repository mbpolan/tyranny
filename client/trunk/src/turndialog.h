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
// turndialog.h: definition of the TurnDialog class.

#ifndef TURNDIALOG_H
#define TURNDIALOG_H

#include <QDialog>
#include <QVector>

namespace Ui {
	class TurnDialog;
}

/**
 * An animated dialog to display which players go when.
 * Make sure to call animate() after you create the dialog and before you
 * actually show it, otherwise no effects will take place. Internally, the
 * dialog is capable of determining when to start animating: if it's hidden,
 * no animations will take place.
 */
class TurnDialog: public QDialog {
	Q_OBJECT

	public:
		/**
		 * Creates a new turn display dialog.
		 * The usernames vector must be mapped to each user's respective index. In other
		 * words, when the game server sends user data, make sure to order the
		 * usernames according to indices. This way, the order vector maps indices to
		 * turn orders, as so:
		 *
		 * 0 (user1) ==> 1  (user1 goes second)
		 * 1 (user4) ==> 3  (user4 goes last)
		 * 2 (user2) ==> 2  (user2 goes third)
		 * 3 (user3) ==> 0  (user3 goes first)
		 *
		 * @param usernames A vector of usernames.
		 * @param order A vector of index-mapped turn orders.
		 * @param parent The parent widget of this dialog.
		 */
		TurnDialog(const QVector<QString> &usernames, const QVector<int> &order, QWidget *parent=NULL);

		/// Begins animating the dialog's components.
		void animate();

	private slots:
		/// Slot for animating the dialog.
		void onTick();

	private:
		Ui::TurnDialog *ui;

		/// Counter of how many times this dialog has animated.
		int m_Loops;

		/// Cache turn orderings.
		QVector<int> m_TurnOrder;
};

#endif
