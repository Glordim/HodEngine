#include "Mainwindow.h"
#include "./ui_Mainwindow.h"

#include "DockableWindow/Contents.h"

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

}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void MainWindow::CloseProject()
{

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
