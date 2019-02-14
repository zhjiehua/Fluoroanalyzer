#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ParaQRGenerator.h"
#include "QRImage.h"

class ParaQRGenerator : public QMainWindow
{
	Q_OBJECT

public:
	ParaQRGenerator(QWidget *parent = Q_NULLPTR);

public slots:
	void on_btnGen_clicked();
	void ComboBoxIndexChanged(int index);

private:
	Ui::ParaQRGeneratorClass ui;
	QRImage *qrImage;
};
