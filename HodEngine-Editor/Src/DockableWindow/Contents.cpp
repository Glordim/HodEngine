#include "Contents.h"
#include "./ui_Contents.h"
#include <windows.h>

#include <QMenu>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
Contents::Contents(QWidget *parent)
	: QDockWidget(parent)
	, _ui(new Ui::Contents)
{
	_ui->setupUi(this);

	auto widget = _ui->treeView;
	widget->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(widget, &QTreeView::customContextMenuRequested, this, &Contents::CustomMenuRequested);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
Contents::~Contents()
{
	delete _ui;
}

void Contents::CustomMenuRequested(const QPoint& position)
{
	QMenu* menu = new QMenu(_ui->treeView);
	QAction* action = nullptr;

	action = menu->addAction("Create png content", [this]()
		{

		});

	connect(action, &QAction::triggered, this, &Contents::CustomMenuActionrequested);



	menu->addAction("Create 2");
	menu->addAction("Create 3");

	menu->popup(_ui->treeView->viewport()->mapToGlobal(position));

	//menu->show();

	qWarning("Right click %i", position);
	OutputDebugString((LPCWSTR)"Right click");
}

void Contents::CustomMenuActionrequested()
{
	qWarning("Right click");
	OutputDebugString((LPCWSTR)"Right click");
}


