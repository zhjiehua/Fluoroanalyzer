#pragma once

#include <QtWidgets/QMainWindow>

class Misc : public QObject// QMainWindow
{
	Q_OBJECT

public:
	//Misc(QWidget *parent = Q_NULLPTR);
	Misc(void);

	static QByteArray QString2Hex(QString str);
	static qint8 ConvertHexChar(qint8 ch);
	static void LeastSquare(QVector<qint32> &myList, double *a, double *b);

public slots:

private:

};
