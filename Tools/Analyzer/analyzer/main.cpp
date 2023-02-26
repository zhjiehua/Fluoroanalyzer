#include "FATool.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	FATool w;
	w.show();
	return a.exec();
}
