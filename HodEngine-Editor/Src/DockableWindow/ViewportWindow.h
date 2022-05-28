#pragma once

#include "DockableWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
	class ViewportWindow;
}
QT_END_NAMESPACE

class QProcess;
class QThread;
class QTcpServer;

#include <QTcpSocket>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class ViewportWindow : public DockableWindow
{
	Q_OBJECT

	META_TYPE(ViewportWindow)

public:

						ViewportWindow(QWidget* parent = nullptr);
						~ViewportWindow();

	void				ResizeEngineWindow(HWND hWnd);

	void				SendEngineRequest(const QString& request, QJsonObject* serializedArgument);

protected:	

	void				resizeEvent(QResizeEvent* event) override;

private:	

	void				OnPlayStopEngineButtonClicked();

	void				StartEngine();
	void				StopEngine();

	void				OnEngineConnect();
	void				OnEngineDisconnect();
	void				OnEngineFinished();
	void				EngineSocketLoop();

private slots:

	void				OnProjectLoaded();
	void				OnProjectUnloaded();

private:

	Ui::ViewportWindow*	_ui;

	QProcess*			_engineProcess = nullptr;
	qint64				_enginePid = 0;
	WId					_engineWindowHandle = 0;

	QTcpServer*			_tcpServer = nullptr;
	QTcpSocket*			_engineSocket = nullptr;
	QThread*			_engineSocketThread = nullptr;
	bool				_quitThread = false;
};
