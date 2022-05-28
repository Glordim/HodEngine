#pragma once

#include <QMainWindow>

#include <../HodEngine/Core/Src/Singleton.h>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class MainWindow;
}
QT_END_NAMESPACE

class DockableWindow;

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class MainWindow : public QMainWindow, public HOD::CORE::Singleton<MainWindow>
{
	Q_OBJECT

public:

								MainWindow(QWidget* parent = nullptr);
								MainWindow(const MainWindow&) = delete;
								MainWindow(MainWindow&&) = delete;
								~MainWindow() override;

	void						operator=(const MainWindow&) = delete;
	void						operator=(MainWindow&&) = delete;

public:

	bool						LoadProjectAtPath(const QString& projectFilePath);

	template<typename __Type__>
	__Type__*					GetOrCreateDockableWindow();

	void						Refresh();

protected:

	void						closeEvent(QCloseEvent* event) override;

private:

	template<typename __Type__>
	void						RegisterDockableWindow(const char* label);
	void						AddDocakbleWindow(DockableWindow* dockableWindow);

	void						NewProject();
	void						OpenProject();

	void						SaveProject();
	void						CloseProject();

	void						Exit();

	void						SetDefaultLayout();
	void						SaveLayout();
	void						LoadLayout();

private:

	Ui::MainWindow*				_ui;
	QVector<DockableWindow*>	_dockWidgets;
};

#include "Mainwindow.inl"
