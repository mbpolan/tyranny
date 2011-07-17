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
// resultdialog.cpp: implementation of the ResultDialog class.

#include "resultdialog.h"

#include "ui/ui_resultdialog.h"

ResultDialog::ResultDialog(int warnCount, int vertexCount, int tcCount, int faceCount,
				   const QVector<Material> &materials, QWidget *parent): QDialog(parent) {
	ui=new Ui::ResultDialog;
	ui->setupUi(this);

	// populate the proper fields
	ui->warnLabel->setText(QString("%1").arg(warnCount));
	ui->vertexLabel->setText(QString("%1").arg(vertexCount));
	ui->tcLabel->setText(QString("%1").arg(tcCount));
	ui->faceLabel->setText(QString("%1").arg(faceCount));
	ui->materialsLabel->setText(QString("%1").arg(materials.size()));

	for (int i=0; i<materials.size(); i++)
		QListWidgetItem *item=new QListWidgetItem(materials[i].diffuseTexture, ui->materialList);
}
