
#include <QMenuBar>

#include "./ui_Mainwindow.h"

///
///@brief 
///
///@tparam __Type__ 
///@param label 
///
template<typename __Type__>
void MainWindow::RegisterDockableWindow(const char* label)
{
	_ui->menuWindow->addAction(label, [this]()
	{
		__Type__* dockableWindow = new __Type__(this);
		AddDocakbleWindow(dockableWindow);
	});
}

template<typename __Type__>
__Type__* MainWindow::GetOrCreateDockableWindow()
{
	for (DockableWindow* dockableWindow : _dockWidgets)
	{
		if (dockableWindow->GetType() == __Type__::_type)
		{
			return static_cast<__Type__*>(dockableWindow);
		}
	}

	__Type__* dockableWindow = new __Type__(this);
	AddDocakbleWindow(dockableWindow);
	return dockableWindow;
}
