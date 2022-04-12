#include "Mainwindow.h"
#include "./ui_Mainwindow.h"

#include "DockableWindow/ContentWindow/ContentWindow.h"
#include "DockableWindow/SceneWindow/SceneWindow.h"
#include "DockableWindow/ViewportWindow.h"
#include "Dialog/NewProjectDialog.h"
#include "Project.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QMenu>

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

template<>
MainWindow* HOD::CORE::Singleton<MainWindow>::_instance = nullptr;

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, _ui(new Ui::MainWindow)
{
	HOD::CORE::Singleton<MainWindow>::_instance = this;

	_ui->setupUi(this);

	QObject::connect(_ui->actionNew_project, &QAction::triggered, this, &MainWindow::NewProject);
	QObject::connect(_ui->actionOpen_project, &QAction::triggered, this, &MainWindow::OpenProject);

	QObject::connect(_ui->actionSave, &QAction::triggered, this, &MainWindow::SaveProject);
	QObject::connect(_ui->actionClose_project, &QAction::triggered, this, &MainWindow::CloseProject);

	QObject::connect(_ui->actionExit, &QAction::triggered, this, &MainWindow::Exit);

	QObject::connect(_ui->actionDefault, &QAction::triggered, this, &MainWindow::SetDefaultLayout);

	RegisterDockableWindow<ContentWindow>("Contents");
	RegisterDockableWindow<SceneWindow>("Scene");

	_ui->actionDefault->trigger();

	Refresh();
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
	delete _ui;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void MainWindow::NewProject()
{
	// Todo check dirty for save
	NewProjectDialog* newProjectDialog = new NewProjectDialog(this);
	newProjectDialog->show();
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void MainWindow::OpenProject()
{
	QString projectFilePath = QFileDialog::getOpenFileName(this, QString(), QString(), "*.hod");

	if (projectFilePath.isEmpty() == false)
	{
		LoadProjectAtPath(projectFilePath);
	}
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void MainWindow::SaveProject()
{
	Project* project = Project::GetInstance();

	if (project->IsDirty() == true)
	{
		project->SaveAtPath(project->GetSavePath());
	}
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void MainWindow::CloseProject()
{
	// Todo check dirty for save

	Project::GetInstance()->Close();

	Refresh();
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void MainWindow::Exit()
{
	// Todo check dirty for save
	close();
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void MainWindow::AddDocakbleWindow(DockableWindow* dockableWindow)
{
	bool projectOpened = Project::GetInstance()->IsOpened();
	dockableWindow->setEnabled(projectOpened);
	addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockableWindow);
	_dockWidgets.push_back(dockableWindow);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void MainWindow::SetDefaultLayout()
{
	for (QDockWidget* dockWidget : _dockWidgets)
	{
		removeDockWidget(dockWidget);
		delete dockWidget;
	}
	_dockWidgets.clear();

	ContentWindow* contentWindow = new ContentWindow(this);
	AddDocakbleWindow(contentWindow);

	ViewportWindow* viewportWindow = new ViewportWindow(this);
	setCentralWidget(viewportWindow);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void MainWindow::Refresh()
{
	bool projectOpened = Project::GetInstance()->IsOpened();
	
	_ui->actionSave->setEnabled(projectOpened);
	_ui->actionClose_project->setEnabled(projectOpened);

	_ui->actionUndo->setEnabled(projectOpened);
	_ui->actionRedo->setEnabled(projectOpened);
	_ui->actionCut->setEnabled(projectOpened);
	_ui->actionCopy->setEnabled(projectOpened);
	_ui->actionPaste->setEnabled(projectOpened);
	_ui->actionDelete->setEnabled(projectOpened);
	_ui->actionSelect_all->setEnabled(projectOpened);

	for (QDockWidget* dockWidget : _dockWidgets)
	{
		dockWidget->setEnabled(projectOpened);
	}

	if (projectOpened == true)
	{
		setWindowTitle("HodEngine - " + Project::GetInstance()->GetName());
	}
	else
	{
		setWindowTitle("HodEngine");
	}

	QString userPreferencesPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

	QFile file(userPreferencesPath);
	if (file.open(QIODevice::ReadOnly) == true)
	{
		QByteArray saveData = file.readAll();
		file.close();

		QJsonDocument jsonDocument = QJsonDocument::fromJson(saveData);
		QJsonObject root = jsonDocument.object();
		QJsonArray recentsArray = root["Recents"].toArray();

		_ui->menuRecent->setEnabled(recentsArray.size() > 0);
		_ui->menuRecent->clear();

		for (qsizetype i = 0; i < recentsArray.size(); ++i)
		{
			QString projectPath = recentsArray[i].toString();

			QFile file(projectPath);
			if (file.exists() == true)
			{
				QAction* action = _ui->menuRecent->addAction(file.fileName(), [this, projectPath]()
				{
					LoadProjectAtPath(projectPath);
				});

				action->setToolTip(projectPath);
			}
		}
	}
	else
	{
		_ui->menuRecent->setEnabled(false);
	}
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool MainWindow::LoadProjectAtPath(const QString& projectFilePath)
{
	// Todo check dirty for save

	CloseProject();

	if (Project::GetInstance()->LoadFromFile(projectFilePath) == false)
	{
		return false;
	}

	QList<QAction*> actions = _ui->menuRecent->actions();

	QString userPreferencesPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

	QJsonObject root;
	QJsonArray recentsArray;
	recentsArray.append(projectFilePath);
	for (qsizetype i = 0; i < actions.size() && i < 5; ++i)
	{
		QString projectPath = actions[i]->toolTip();

		bool alreadyExist = false;
		for (qsizetype j = 0; j < recentsArray.size(); ++j)
		{
			if (recentsArray[j].toString() == projectPath)
			{
				alreadyExist = true;
				break;
			}
		}

		if (alreadyExist == false)
		{
			recentsArray.append(actions[i]->toolTip());
		}
	}
	root["Recents"] = recentsArray;

	QFile file(userPreferencesPath);
	if (file.open(QIODevice::WriteOnly) == true)
	{
		file.write(QJsonDocument(root).toJson());
		file.close();
	}

	Refresh();

	return true;
}
