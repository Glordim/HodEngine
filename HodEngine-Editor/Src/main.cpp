
#include "Mainwindow.h"
#include "ContentDatabase.h"
#include "Project.h"
#include "Contents/ContentFactory.h"

#include <QApplication>
#include "Style/DarkStyle.h"
#include "ApplicationDescription.h"

#include <QProcess>
#include <QDir>

int main(int argc, char** argv)
{
	QApplication application(argc, argv);
	QApplication::setStyle(new DarkStyle);

	Project::CreateInstance();
	ContentDataBase::CreateInstance();
	ContentFactory::Init();

	QString editorPath = QCoreApplication::applicationDirPath();
	QString applicationDumpFilePath = editorPath + "/ApplicationDescription.json";

	QDir engineDir(editorPath);
	engineDir.cdUp();

	QStringList arguments;
	arguments.push_back("--toolDump");
	arguments.push_back(applicationDumpFilePath);

	QProcess* applicationProcess = new QProcess();
	applicationProcess->setProgram(engineDir.absolutePath() + "/../Debug/HodEngine-Application.exe");
	applicationProcess->setArguments(arguments);
	applicationProcess->start();
	applicationProcess->waitForFinished();

	ApplicationDescription::CreateInstance();
	ApplicationDescription::GetInstance()->LoadFromFile(applicationDumpFilePath);

	MainWindow* mainWindow = new MainWindow();
	mainWindow->setWindowIcon(application.style()->standardIcon(QStyle::SP_DesktopIcon));

	mainWindow->show();
	return application.exec();
}
