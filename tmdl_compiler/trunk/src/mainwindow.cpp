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
#include "resultdialog.h"

#include "ui/ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
	ui=new Ui::MainWindow;
	ui->setupUi(this);

	// connect signals
	connect(ui->inputBrowse, SIGNAL(clicked()), this, SLOT(onInputBrowse()));
	connect(ui->outputBrowse, SIGNAL(clicked()), this, SLOT(onOutputBrowse()));
	connect(ui->compileBtn, SIGNAL(clicked()), this, SLOT(onCompile()));
	connect(ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));

	// initialize other variables
	m_Compiler=NULL;
	m_WarnCount=0;
}

void MainWindow::onAbout() {
	QMessageBox::about(this, tr("About"), tr("Tyranny Model Compiler 1.0\n\n"
				 "This is a compiler that produces Tyranny client models from the universal Wavefront OBJ models."));
}

void MainWindow::onInputBrowse() {
	// present the user with the file dialog
	QString input=QFileDialog::getOpenFileName(this, tr("Select target"),
								 QDesktopServices::storageLocation(QDesktopServices::DesktopLocation),
								 tr("OBJ Models (*.obj)"));
	if (!input.isNull())
		ui->inputEdit->setText(input);
}

void MainWindow::onOutputBrowse() {
	// present the user with the file dialog
	QString input=QFileDialog::getOpenFileName(this, tr("Select destination"),
								 QDesktopServices::storageLocation(QDesktopServices::DesktopLocation),
								 tr("Tyranny Models (*.tmdl)"));
    if (!input.isNull())
	    ui->outputEdit->setText(input);
}

void MainWindow::onCompile() {
	// make sure the input path is valid
	if (ui->inputEdit->text().isEmpty()) {
		QMessageBox::critical(this, tr("Error"), tr("You must specify a valid path to an OBJ model."));
		return;
	}

	// make sure the output path is valid
	if (ui->outputEdit->text().isEmpty()) {
		QMessageBox::critical(this, tr("Error"), tr("You must specify a valid path to the output Tyranny model."));
		return;
	}

	ui->logEdit->clear();

	// free any previously allocated compiler objects
	if (m_Compiler)
		delete m_Compiler;

	// create a new compiler object and connect signals
	m_Compiler=new Compiler(ui->inputEdit->text(), ui->outputEdit->text());

	connect(m_Compiler, SIGNAL(started(QDateTime)), this, SLOT(onStarted(QDateTime)));
	connect(m_Compiler, SIGNAL(completed(QDateTime)), this, SLOT(onCompleted(QDateTime)));
	connect(m_Compiler, SIGNAL(error(QString)), this, SLOT(onError(QString)));
	connect(m_Compiler, SIGNAL(message(QString)), this, SLOT(onMessage(QString)));
	connect(m_Compiler, SIGNAL(warning(QString)), this, SLOT(onWarning(QString)));

	// let's go!
	m_Compiler->start();
}

/// Handler for started signal from Compiler class.
void MainWindow::onStarted(const QDateTime &beginTime) {
	QString entry="Compile started on: "+beginTime.toString()+"\n";

	ui->logEdit->setTextColor(Qt::black);
	ui->logEdit->append(entry);
}

/// Handler for completed signal from Compiler class.
void MainWindow::onCompleted(const QDateTime &endTime) {
	QString entry="\nCompile ended on: "+endTime.toString()+"\n";

	ui->logEdit->setTextColor(Qt::black);
	ui->logEdit->append(entry);

	// show the status dialog
	ResultDialog rd(m_WarnCount, m_Compiler->getVertexCount(), m_Compiler->getTextureCoordinateCount(),
			    m_Compiler->getFaceCount(), m_Compiler->getMaterialsData());
	rd.exec();
}

/// Handler for general messages from Compiler class.
void MainWindow::onMessage(const QString &str) {
	ui->logEdit->setTextColor(Qt::black);
	ui->logEdit->append(str);
}

/// Handler for error messages from Compiler class.
void MainWindow::onError(const QString &str) {
	ui->logEdit->setTextColor(Qt::red);
	ui->logEdit->append(str);
}

/// Handler for warning messages from Compiler class.
void MainWindow::onWarning(const QString &str) {
	ui->logEdit->setTextColor(Qt::yellow);
	ui->logEdit->append(str);

	m_WarnCount++;
}
