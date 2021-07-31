#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class MainWindow;
}
QT_END_NAMESPACE

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:

							MainWindow(QWidget* parent = nullptr);
							MainWindow(const MainWindow&) = delete;
							MainWindow(MainWindow&&) = delete;
							~MainWindow() override;

	void					operator=(const MainWindow&) = delete;
	void					operator=(MainWindow&&) = delete;

public:

	bool					LoadProjectAtPath(const QString& projectFilePath);

	void					Refresh();

private:

	template<typename __Type__>
	void					RegisterDockableWindow(const char* label);
	void					AddDocakbleWindow(QDockWidget* dockWidget);

	void					NewProject();
	void					OpenProject();

	void					SaveProject();
	void					CloseProject();

	void					Exit();

	void					SetDefaultLayout();

private:

	Ui::MainWindow*			_ui;
	QVector<QDockWidget*>	_dockWidgets;
};

#include "Mainwindow.inl"
