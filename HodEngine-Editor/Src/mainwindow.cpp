#include "Mainwindow.h"
#include "./ui_Mainwindow.h"

#include "DockableWindow/Contents.h"
#include "Dialog/NewProjectDialog.h"
#include "Project.h"

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, _ui(new Ui::MainWindow)
{
	_ui->setupUi(this);

	QObject::connect(_ui->actionNew_project, &QAction::triggered, this, &MainWindow::NewProject);
	QObject::connect(_ui->actionOpen_project, &QAction::triggered, this, &MainWindow::OpenProject);
	QObject::connect(_ui->actionRecent, &QAction::triggered, this, &MainWindow::RecentsProject);

	QObject::connect(_ui->actionSave, &QAction::triggered, this, &MainWindow::SaveProject);
	QObject::connect(_ui->actionClose_project, &QAction::triggered, this, &MainWindow::CloseProject);

	QObject::connect(_ui->actionExit, &QAction::triggered, this, &MainWindow::Exit);

	QObject::connect(_ui->actionDefault, &QAction::triggered, this, &MainWindow::SetDefaultLayout);

	RegisterDockableWindow<Contents>("Contents");

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

}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void MainWindow::RecentsProject()
{

}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void MainWindow::SaveProject()
{
	if (_project->IsDirty() == true)
	{
		_project->SaveAtPath(_project->GetSavePath());
	}
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void MainWindow::CloseProject()
{
	// Todo check dirty for save

	delete _project;
	_project = nullptr;

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
void MainWindow::SetDefaultLayout()
{
	for (QDockWidget* dockWidget : _dockWidgets)
	{
		removeDockWidget(dockWidget);
		delete dockWidget;
	}
	_dockWidgets.clear();

	Contents* contents = new Contents(this);
	addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, contents);
	_dockWidgets.push_back(contents);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void MainWindow::Refresh()
{
	bool projectOpened = (_project != nullptr);
	
	_ui->actionSave->setEnabled(projectOpened);
	_ui->actionClose_project->setEnabled(projectOpened);

	_ui->actionUndo->setEnabled(projectOpened);
	_ui->actionRedo->setEnabled(projectOpened);
	_ui->actionCut->setEnabled(projectOpened);
	_ui->actionCopy->setEnabled(projectOpened);
	_ui->actionPaste->setEnabled(projectOpened);
	_ui->actionDelete->setEnabled(projectOpened);
	_ui->actionSelect_all->setEnabled(projectOpened);

	if (projectOpened == true)
	{
		setWindowTitle("HodEngine - " + _project->GetName());
	}
	else
	{
		setWindowTitle("HodEngine");
	}
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool MainWindow::LoadProjectAtPath(const QString& projectFilePath)
{
	// Todo check dirty for save

	_project = new Project();
	if (_project->LoadFromFile(projectFilePath) == false)
	{
		return false;
	}

	Refresh();
}
