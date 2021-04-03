#include "Viewport.h"
#include "./ui_Viewport.h"

#include <QProcess>
#include <QThread>
#include <QTcpServer>

#include <QJsonDocument>
#include <QJsonObject>

#include <Windows.h>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
Viewport::Viewport(QWidget *parent)
	: QDockWidget(parent)
	, _ui(new Ui::Viewport)
{
	_ui->setupUi(this);

	_tcpServer = new QTcpServer(this);
	_tcpServer->listen();
	QObject::connect(_tcpServer, &QTcpServer::newConnection, this, &Viewport::OnEngineConnect);

	QStringList arguments;
	arguments.push_back("-Editor");
	arguments.push_back(QString::number(_ui->frameContainer->winId()));
	arguments.push_back("-Port");
	arguments.push_back(QString::number(_tcpServer->serverPort()));

	_engineProcess = new QProcess(this);
	_engineProcess->setProgram("G:\\HodEngine\\Build\\Debug\\HodEngine.exe");
	_engineProcess->setArguments(arguments);
	_engineProcess->setWorkingDirectory("C:\\Users\\antho\\Desktop\\Hod\\Resources");
	QObject::connect(_engineProcess, &QProcess::finished, this, &Viewport::OnEngineFinished);

	QObject::connect(_ui->restartButton, &QToolButton::clicked, this, &Viewport::OnPlayStopEngineButtonClicked);

	StartEngine();
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
Viewport::~Viewport()
{
	if (_engineProcess != nullptr)
	{
		_engineProcess->terminate();
		delete _engineProcess;
	}

	delete _ui;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Viewport::OnPlayStopEngineButtonClicked()
{
	QProcess::ProcessState state = _engineProcess->state();

	if (state == QProcess::ProcessState::NotRunning)
	{
		StartEngine();
	}
	else
	{
		StopEngine();
	}
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Viewport::StartEngine()
{
	QProcess::ProcessState state = _engineProcess->state();

	if (state == QProcess::ProcessState::NotRunning)
	{
		_engineProcess->start();
		_ui->statusLabel->setText("Waiting engine process...");
	}
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Viewport::StopEngine()
{
	QProcess::ProcessState state = _engineProcess->state();

	if (state != QProcess::ProcessState::NotRunning)
	{
		_engineProcess->kill();
		_engineProcess->waitForFinished();
	}
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Viewport::OnEngineConnect()
{
	_ui->statusLabel->setText("Connected");

	_engineSocket = _tcpServer->nextPendingConnection();

	QObject::connect(_engineSocket, &QAbstractSocket::disconnected, this, &Viewport::StopEngine);

	_engineSocketThread = QThread::create(std::bind(&Viewport::EngineSocketLoop, this));
	_engineSocketThread->start();
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Viewport::EngineSocketLoop()
{
	// May be setParent to engine window here ?
	_quitThread = false;
	while (_quitThread == false)
	{
		qint64 availableByte = _engineSocket->bytesAvailable();

		if (availableByte > 0)
		{
			//char* buffer = new char[availableByte];
			//_engineSocket->read(buffer, availableByte);

			ushort messageSize;
			_engineSocket->read((char*)(&messageSize), sizeof(ushort));

			QByteArray message = _engineSocket->read(messageSize);

			QJsonDocument document = QJsonDocument::fromJson(message);
			QJsonObject root = document.object();
			QString command = root.value("Command").toString();

			if (command == "SetWindowHandle")
			{
				_engineWindowHandle = root.value("WindowHandle").toInteger();

				SetParent((HWND)_engineWindowHandle, (HWND)_ui->frameContainer->winId()); //a will be the new parent b
				DWORD style = GetWindowLong((HWND)_engineWindowHandle, GWL_STYLE); //get the b style
				style &= ~(WS_POPUP | WS_CAPTION); //reset the "caption" and "popup" bits
				style |= WS_CHILD; //set the "child" bit
				SetWindowLong((HWND)_engineWindowHandle, GWL_STYLE, style); //set the new style of b

				QSize size = _ui->frameContainer->size();
				QPoint pos = QWidget::mapFromGlobal(_ui->frameContainer->pos());
				pos = _ui->frameContainer->pos();

				SetWindowPos((HWND)_engineWindowHandle, NULL, 0, 0, size.width(), size.height(), 0);
				ShowWindow((HWND)_engineWindowHandle, SW_SHOWNORMAL);
			}
		}
	}
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Viewport::OnEngineFinished()
{
	if (_engineSocketThread != nullptr)
	{
		_quitThread = true;
		if (_engineSocketThread->wait(5000) == false)
		{
			_engineSocketThread->terminate();
			_engineSocketThread->wait();
		}
		delete _engineSocketThread;
		_engineSocketThread = nullptr;
	}

	if (_engineSocket != nullptr)
	{
		_engineSocket->close();
		delete _engineSocket;
		_engineSocket = nullptr;
	}

	_ui->statusLabel->setText("Disconnected");
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Viewport::resizeEvent(QResizeEvent* event)
{
	QDockWidget::resizeEvent(event);

	QSize size = _ui->frameContainer->size();
	QPoint pos = QWidget::mapFromGlobal(_ui->frameContainer->pos());
	pos = _ui->frameContainer->pos();

	SetWindowPos((HWND)_engineWindowHandle, NULL, 0, 0, size.width(), size.height(), 0);
}
