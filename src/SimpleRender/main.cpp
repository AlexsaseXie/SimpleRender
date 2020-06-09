#include "SimpleRender.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SimpleRender w;
	w.show();
	return a.exec();
}
