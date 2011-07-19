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
// tokenchooser.h: definition of the TokenChooser class.

#ifndef TOKENCHOOSER_H
#define TOKENCHOOSER_H

#include <QDialog>

namespace Ui {
	class TokenChooser;
}

class TokenChooser: public QDialog {
	Q_OBJECT

	public:
		/**
		 * Creates a new token chooser dialog.
		 *
		 * @param usernames Vector of ordered usernames, by turn order.
		 */
		TokenChooser(const QVector<QString> &usernames, QWidget *parent=NULL);

		/**
		 * Toggles the token buttons.
		 *
		 * @param enable true to enable, false to disable.
		 */
		void setButtonsEnabled(bool enable);

		/**
		 * Updates the chosen token for the given username.
		 *
		 * @param username The username to modify.
		 * @param piece Piece index [1,6]
		 */
		void updateUsername(const QString &username, int piece);

	signals:
		/// Signal emitted when the user has chosen a piece.
		void pieceChosen(int piece);

	private slots:
		/// Handler for disabling the dialog when the user chooses a token.
		void onTokenChosen(int sender);

	private:
		Ui::TokenChooser *ui;

		/// Cached copy of usernames.
		QVector<QString> m_Usernames;
};

#endif
