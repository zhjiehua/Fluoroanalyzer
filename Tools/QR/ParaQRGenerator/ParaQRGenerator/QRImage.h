#pragma once

#include <QtWidgets/QMainWindow>
#include <QPainter>
#include "../../qrencode-4.0.2/qrencode.h"


class QRImage : public QMainWindow
{
	Q_OBJECT

public:
	QRImage(QWidget *parent = Q_NULLPTR);

	void setString(QString str);
	int getQRWidth() const;
	bool saveImage(QString name, int size);

protected:
	void paintEvent(QPaintEvent *);
	QSize sizeHint() const;
	QSize minimumSizeHint() const;

private:
	void draw(QPainter &painter, int width, int height);
	QString string;
	QRcode *qr;
};
