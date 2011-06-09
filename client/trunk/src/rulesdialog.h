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
// rulesdialog: definition of the RulesDialog class.

#ifndef RULESDIALOG_H
#define RULESDIALOG_H

#include <QDialog>

namespace Ui {
	class RulesDialog;
}

/**
 * A dialog containing possible user-defined game room rules.
 * A user has the option of setting certain rules that all other players must obey
 * if they choose to join the given room. All such options are represented as easy
 * to use widgets in this dialog.
 */
class RulesDialog: public QDialog {
	Q_OBJECT

	public:
		/**
		 * Data contained within the RulesDialog dialog.
		 * Instead of a multidude of methods to get the data from the rules dialog, a Settings
		 * object is returned that contains a complete outline of the chosen rules.
		 */
		class Settings {
			public:
				enum PropertyRedistribution { Random, ReturnToBank };

			public:
				/**
				 * Constructs a settings object with the given data.
				 *
				 * @param maxTurns The maximum number of game turns.
				 * @param maxHumans The maximum number of human players.
				 * @param freeParkReward The reward money earned for stepping on Free Parking.
				 * @param redistMethod Property redistribution method after a player bankrupts.
				 * @param incomeTaxChoice Does stepping on Income Tax gives the player a choice of fee?
				 * @param password Room password, or empty string for none.
				 * @param onlyFriends Whether or not to allow only friends to join.
				 */
				Settings(int maxTurns, int maxHumans, int freeParkReward, const PropertyRedistribution &redistMethod,
					   bool incomeTaxChoice, const QString &password, bool onlyFriends);

				/**
				 * Returns the maximum number of game turns.
				 * @return Max turns.
				 */
				int getMaxTurns() const { return m_MaxTurns; }

				/**
				 * Returns the maximum number of human players.
				 * @return Max human players.
				 */
				int getMaxHumans() const { return m_MaxHumans; }

				/**
				 * Returns the reward for setting on Free Parking.
				 * @return Free Parking reward.
				 */
				int getFreeParkingReward() const { return m_FreeParkReward; }

				/**
				 * Returns the property redistribution method following player bankruptcy.
				 * @return Property redistribution method.
				 */
				PropertyRedistribution getPropertyRedistributionMethod() const { return m_PropRedist; }

				/**
				 * Returns whether or not a player is given a choice of fee for stepping on Income Tax.
				 * @return Income Tax fee choice.
				 */
				bool getIncomeTaxChoice() const { return m_IncomeTaxChoice; }

				/**
				 * Returns the room password.
				 * @return Room password.
				 */
				QString getRoomPassword() const { return m_Password; }

				/**
				 * Returns whether or not only friends can join the room.
				 * @return True if yes, false otherwise.
				 */
				bool getOnlyFriends() const { return m_OnlyFriends; }

			private:
				int m_MaxTurns;
				int m_MaxHumans;
				int m_FreeParkReward;
				PropertyRedistribution m_PropRedist;
				bool m_IncomeTaxChoice;
				QString m_Password;
				bool m_OnlyFriends;
		};

	public:
		/**
		 * Creates a default, blank rules dialog.
		 *
		 * @param parent The parent window for this dialog.
		 */
		RulesDialog(QWidget *parent=NULL);

		/**
		 * Returns the defined game room settings as a Settings object.
		 *
		 * @return A Settings object.
		 */
		Settings getDefinedSettings() const ;

	private:
		Ui::RulesDialog *ui;
};

#endif
