#include <QTranslator>

#include "QApplication"
#include "../include/WASettings.hpp"
#include "../include/WAwindow.hpp"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	QTranslator qtTranslator;
	if (qtTranslator.load("Translation_zh_CN.qm"))
		QApplication::installTranslator(&qtTranslator);
	WA::Init();
	WAwindow w(&a);
	int returnvalue = QApplication::exec();
	WA::End();
	return returnvalue;
}
