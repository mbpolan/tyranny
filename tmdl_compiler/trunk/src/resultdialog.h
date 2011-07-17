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
// resultdialog.h: definition of the ResultDialog class.

#ifndef RESULTDIALOG_H
#define RESULTDIALOG_H

#include <QDialog>

#include "tmdl.h"

namespace Ui {
	class ResultDialog;
}

/**
 * A simple dialog for displaying post-compilation data.
 * This dialog lets the user review the final results of a compile run
 * on a source model.
 */
class ResultDialog: public QDialog {
	Q_OBJECT

	public:
		/**
		 * Creates a new result dialog with the given data.
		 *
		 * @param warnCount The amount of warnings that occurred.
		 * @param vertexCount The amount of vertices in the model.
		 * @param tcCount The amount of texture coordinates in the model.
		 * @param faceCount The amount of faces in the model.
		 * @param materials Data about materials relevant to this model.
		 * @param parent The parent widget for this dialog.
		 */
		ResultDialog(int warnCount, int vertexCount, int tcCount, int faceCount, const QVector<Material> &materials, QWidget *parent=NULL);

	private:
		Ui::ResultDialog *ui;
};

#endif
