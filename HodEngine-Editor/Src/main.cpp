
#include "Mainwindow.h"
#include "ContentDatabase.h"


#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication application(argc, argv);

	MainWindow mainWindow;
	mainWindow.show();
	/*
	ContentDataBase base;
	base.GenerateContentFolder();*/

	return application.exec();
}
