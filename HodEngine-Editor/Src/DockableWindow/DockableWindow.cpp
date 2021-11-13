#include "DockableWindow.h"

#include <QProcess>
#include <QThread>
#include <QTcpServer>

#include <QJsonDocument>
#include <QJsonObject>

#include <Windows.h>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
DockableWindow::DockableWindow(QWidget *parent)
	: QDockWidget(parent)
{

}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
DockableWindow::~DockableWindow()
{

}
