#include "Contents.h"
#include "./ui_Contents.h"
#include <windows.h>

#include "../ContentDatabase.h"
#include "../Contents/TextureContent.h"

#include <QMenu>
#include <QFileDialog>
#include <QTreeWidgetItem>
#include <QStandardItem>

#include <QListView>
#include <QTableView>
#include <QSplitter>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
Contents::Contents(QWidget* parent)
	: QDockWidget(parent)
	, _ui(new Ui::Contents)
	, _onAddContentSlot(std::bind(&Contents::OnAddContent, this, std::placeholders::_1))
	, _onRemoveContentSlot(std::bind(&Contents::OnRemoveContent, this, std::placeholders::_1))
	, _onContentChangeSlot(std::bind(&Contents::OnContentChange, this, std::placeholders::_1))
{
	_ui->setupUi(this);

	_ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
	_treeViewModel = new QStandardItemModel(_ui->treeView);
	_ui->treeView->setModel(_treeViewModel);
	_treeViewModel->setColumnCount(1);

	QObject::connect(_ui->treeView, &QTreeView::customContextMenuRequested, this, &Contents::CustomMenuRequested);

	ContentDataBase::GetInstance()->GetAddContentSignal().Connect(_onAddContentSlot);
	ContentDataBase::GetInstance()->GetRemoveContentSignal().Connect(_onRemoveContentSlot);
	ContentDataBase::GetInstance()->GetContentChangeSignal().Connect(_onContentChangeSlot);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
Contents::~Contents()
{
	delete _ui;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Contents::CustomMenuRequested(const QPoint& position)
{
	QMenu* menu = new QMenu(_ui->treeView);
	QMenu* create = menu->addMenu("Create");
	QMenu* import = menu->addMenu("Import");
	import->addAction("Texture", [this]()
	{
		QString textureFilePath = QFileDialog::getOpenFileName(this);

		ContentDataBase::GetInstance()->Import<TextureContent>(textureFilePath);
	});

	menu->popup(_ui->treeView->viewport()->mapToGlobal(position));
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Contents::OnAddContent(Content* content)
{
	_ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);

	QStandardItem* item = new QStandardItem(content->GetName());

	_treeViewModel->appendRow(item);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Contents::OnRemoveContent(Content* content)
{
	// refresh view
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Contents::OnContentChange(Content* content)
{
	// refresh view
}
