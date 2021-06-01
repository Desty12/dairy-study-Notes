#include "tsimplifycurvedlg.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	TSimplifyCurveDlg w;
	w.show();
	return a.exec();
}
