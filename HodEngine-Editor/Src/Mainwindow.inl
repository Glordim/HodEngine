
#include <QMenuBar>

#include "./ui_Mainwindow.h"

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
template<typename __Type__>
void MainWindow::RegisterDockableWindow(const char* label)
{
	_ui->menuWindow->addAction(label, [this]()
	{
		QDockWidget* dockWidget = new __Type__(this);
		AddDocakbleWindow(dockWidget);
	});
}
