#include "ParaQRGenerator.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ParaQRGenerator w;
	w.show();
	return a.exec();
}
