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
// mainwindow.cpp: implementation of the MainWindow class.

#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>

#include "mainwindow.h"
#include "model.h"

#include "ui/ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
	ui=new Ui::MainWindow;
	ui->setupUi(this);

	// alloate a new scene
	m_Scene=new Scene(this);
	ui->sceneLayout->addWidget(m_Scene);

	// connect signals
	connect(ui->actionOpen_model, SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));
	connect(ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(ui->pitchSlider, SIGNAL(valueChanged(int)), this, SLOT(onPitchSlider(int)));
	connect(ui->yawSlider, SIGNAL(valueChanged(int)), this, SLOT(onYawSlider(int)));
	connect(ui->rollSlider, SIGNAL(valueChanged(int)), this, SLOT(onRollSlider(int)));
	connect(ui->pitchSB, SIGNAL(valueChanged(double)), this, SLOT(onPitchSpinBox(double)));
	connect(ui->yawSB, SIGNAL(valueChanged(double)), this, SLOT(onYawSpinBox(double)));
	connect(ui->rollSB, SIGNAL(valueChanged(double)), this, SLOT(onRollSpinBox(double)));
}

void MainWindow::onOpen() {
	QString path=QFileDialog::getOpenFileName(this, tr("Open Model"),
								QDesktopServices::storageLocation(QDesktopServices::DesktopLocation),
								tr("Tyranny models (*.tmdl)"));
	if (!path.isNull()) {
		// load the model
		Model *model=new Model(this);
		QStringList errors;
		if (!model->load(path, errors)) {
			QMessageBox::critical(this, tr("Error"), tr("Unable to load model."));
			delete model;
		}

		else {
			QString list="";
			for (int i=0; i<errors.size(); i++)
				list+=errors[i]+'\n';
			if (!list.isEmpty())
				QMessageBox::warning(this, tr("Warnings"), QString("The following errors were encountered while loading the model:\n")+list);

			m_Scene->setModel(model);
		}
	}
}

void MainWindow::onAbout() {
	QString msg="Tyranny Model Viewer 1.0\n\nA simple viewer for Tyranny client models (.TMDL).\n\n";
	msg+="Keyboard controls:\n* Up/Down: zoom in/out\n* A/D: shift left/right\n* W/S: shift up/down";

	QMessageBox::about(this, tr("About"), msg);
}

void MainWindow::onPitchSlider(int val) {
	ui->pitchSB->setValue(val);
	m_Scene->setPitch(val);
}

void MainWindow::onYawSlider(int val) {
	ui->yawSB->setValue(val);
	m_Scene->setYaw(val);
}

void MainWindow::onRollSlider(int val) {
	ui->rollSB->setValue(val);
	m_Scene->setRoll(val);
}

void MainWindow::onPitchSpinBox(double val) {
	ui->pitchSlider->setValue(int(val));
	m_Scene->setPitch(val);
}

void MainWindow::onYawSpinBox(double val) {
	ui->yawSlider->setValue(int(val));
	m_Scene->setYaw(val);
}

void MainWindow::onRollSpinBox(double val) {
	ui->rollSlider->setValue(int(val));
	m_Scene->setRoll(val);
}
