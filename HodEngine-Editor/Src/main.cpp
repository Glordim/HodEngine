
#include "Mainwindow.h"
#include "ContentDatabase.h"
#include "Project.h"
#include "Contents/ContentFactory.h"

#include <QApplication>
#include "Style/DarkStyle.h"
#include "Style/framelesswindow/framelesswindow.h"

int main(int argc, char** argv)
{
	QApplication application(argc, argv);
	QApplication::setStyle(new DarkStyle);

	Project::CreateInstance();
	ContentDataBase::CreateInstance();
	ContentFactory::Init();

	FramelessWindow framelessWindow;

	MainWindow* mainWindow = new MainWindow();
	mainWindow->setWindowIcon(application.style()->standardIcon(QStyle::SP_DesktopIcon));

	QObject::connect(mainWindow, &MainWindow::windowTitleChanged, &framelessWindow, &FramelessWindow::setWindowTitle);
	QObject::connect(mainWindow, &MainWindow::windowIconChanged, &framelessWindow, &FramelessWindow::setWindowIcon);

	framelessWindow.setContent(mainWindow);
	framelessWindow.show();

	mainWindow->Refresh();
	framelessWindow.setWindowTitle(mainWindow->windowTitle());
	framelessWindow.setWindowIcon(mainWindow->windowIcon());

	return application.exec();
}
