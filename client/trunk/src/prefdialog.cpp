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
// prefdialog.cpp: implementation of the PrefDialog class.

#include "prefdialog.h"

#include "ui/ui_prefdialog.h"

PrefDialog::PrefDialog(PrefDialog::Data *data, QWidget *parent): QDialog(parent) {
	ui=new Ui::PrefDialog;
	ui->setupUi(this);

	// if a data pointer was passed, synthesize the values
	if (data) {
		ui->ipEdit->setText(data->getIP());
		ui->portEdit->setValue(data->getPort());

		QVector<QPair<QString, int> > list=data->getServers();
		for (int i=0; i<list.size(); i++) {
			QTreeWidgetItem *item=new QTreeWidgetItem(ui->serverList);
			item->setText(0, list[i].first);
			item->setText(1, QString("%1").arg(list[i].second));
		}
    	}
}

PrefDialog::Data* PrefDialog::getPreferencesData() const {
	QVector<QPair<QString, int> > list;

	// iterate over the saved servers
	for (int i=0; i<ui->serverList->topLevelItemCount(); i++) {
		QString ip=ui->serverList->topLevelItem(i)->text(0);
		int port=ui->serverList->topLevelItem(i)->text(1).toInt();

		list.push_back(QPair<QString, int>(ip, port));
	}

	return new PrefDialog::Data(ui->ipEdit->text(), ui->portEdit->value(), list);
}
