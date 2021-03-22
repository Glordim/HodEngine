#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class MainWindow;
}
QT_END_NAMESPACE

class Project;

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:

							MainWindow(QWidget* parent = nullptr);
							~MainWindow();

	bool					LoadProjectAtPath(const QString& projectFilePath);

private:

	template<typename __Type__>
	void					RegisterDockableWindow(const char* label);

	void					NewProject();
	void					OpenProject();
	void					RecentsProject();

	void					SaveProject();
	void					CloseProject();

	void					Exit();

	void					SetDefaultLayout();

	void					Refresh();

private:

	Ui::MainWindow*			_ui;
	QVector<QDockWidget*>	_dockWidgets;

	Project*				_project = nullptr;
};

#include "Mainwindow.inl"
