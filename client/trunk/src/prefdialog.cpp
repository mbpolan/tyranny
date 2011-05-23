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
