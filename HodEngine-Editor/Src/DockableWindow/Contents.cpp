
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
#include <QStringList>
#include <String>

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

	_treeViewModel = new QStandardItemModel();
	_treeViewModel->setColumnCount(1);

	_ui->treeView->setHeaderHidden(true);
	_ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
	_ui->treeView->setModel(_treeViewModel);
	_ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);

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
	QModelIndex index = _ui->treeView->indexAt(position);
	QStandardItem* item = __nullptr;

	if (index.isValid())
	{
		item = _treeViewModel->itemFromIndex(index);

		if (item != nullptr)
		{
			menu->addAction("Delete", [this]()
			{
				//ContentDataBase::GetInstance()->GetContent()
			});
		}
	}
	else
	{
		QMenu* create = menu->addMenu("Create");
		QMenu* import = menu->addMenu("Import");
		import->addAction("Texture", [this]()
		{
			QString textureFilePath = QFileDialog::getOpenFileName(this, "Select a Texture file", Project::GetInstance()->GetAssetsFolderPath(), "*.png");

			ContentDataBase::GetInstance()->Import<TextureContent>(textureFilePath);
		});
	}
	menu->popup(_ui->treeView->viewport()->mapToGlobal(position));
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Contents::OnAddContent(Content* content)
{
	const QString& contentPath = content->GetPath();
	qsizetype offset = contentPath.indexOf("/Contents/");
	if (offset == -1)
	{
		qWarning("Contents::OnAddContent::Content out of folder Contents");
		return;
	}

	QStringList splitPath = contentPath.mid(offset + strlen("/Contents/")).split('/');

	QStandardItem* folder = _treeViewModel->invisibleRootItem();

	qsizetype splitPathCount = splitPath.length();
	for (qsizetype splitPathIndex = 0; splitPathIndex < splitPathCount - 1; ++splitPathIndex) // -1 to iterate only on intermediate folders
	{
		const QString& name = splitPath.at(splitPathIndex);

		QStandardItem* childFolder = nullptr;

		int childCount = folder->rowCount();
		for (int childIndex = 0; childIndex < childCount; ++childIndex)
		{
			QStandardItem* child = folder->child(childIndex);
			if (child->text() == name)
			{
				childFolder = child;
				break;
			}
		}

		if (childFolder == nullptr) // if sub dir does'nt exist, create it
		{
			childFolder = new QStandardItem(name);
			folder->appendRow(childFolder);
		}

		folder = childFolder;
	}

	QStandardItem* item = new QStandardItem(splitPath.last());
	folder->appendRow(item);
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
