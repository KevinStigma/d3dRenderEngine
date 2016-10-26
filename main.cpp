#include "d3drenderengine.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	d3dRenderEngine w;
	w.show();
	return a.exec();
}
