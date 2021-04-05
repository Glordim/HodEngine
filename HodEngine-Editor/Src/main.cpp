
#include "Mainwindow.h"
#include "ContentDatabase.h"
#include "Project.h"

#include <QApplication>

int main(int argc, char** argv)
{
	QApplication application(argc, argv);

	Project::CreateInstance();
	ContentDataBase::CreateInstance();

	MainWindow mainWindow;
	mainWindow.show();

	return application.exec();
}
