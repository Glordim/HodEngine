#pragma once

#include <QDockWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class Viewport;
}
QT_END_NAMESPACE

class QProcess;
class QThread;
class QTcpServer;

#include <QTcpSocket>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class Viewport : public QDockWidget
{
	Q_OBJECT

public:

					Viewport(QWidget* parent = nullptr);
					~Viewport();

	void			ResizeEngineWindow(HWND hWnd);

protected:

	void			resizeEvent(QResizeEvent* event) override;

private:

	void			OnPlayStopEngineButtonClicked();

	void			StartEngine();
	void			StopEngine();

	void			OnEngineConnect();
	void			OnEngineDisconnect();
	void			OnEngineFinished();
	void			EngineSocketLoop();

private:

	Ui::Viewport*	_ui;

	QProcess*		_engineProcess = nullptr;
	qint64			_enginePid = 0;
	WId				_engineWindowHandle = 0;

	QTcpServer*		_tcpServer = nullptr;
	QTcpSocket*		_engineSocket = nullptr;
	QThread*		_engineSocketThread = nullptr;
	bool			_quitThread = false;
};
