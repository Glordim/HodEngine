
#include "SceneWindow.h"
#include "./ui_SceneWindow.h"
#include <windows.h>

#include "SceneTreeViewItem.h"

#include <QMenu>
#include <QFileDialog>
#include <QTreeWidgetItem>

#include <QListView>
#include <QTableView>
#include <QTreeView>
#include <QStringList>
#include <String>
#include <QMessageBox>
#include <QShortcut>
#include <QDesktopServices>

///
///@brief Construct a new SceneWindow:: SceneWindow object
///
///@param parent 
///
SceneWindow::SceneWindow(QWidget* parent)
: DockableWindow(parent)
, _ui(new Ui::SceneWindow)
, _onLoadProjectSlot(std::bind(&SceneWindow::OnLoadProject, this, std::placeholders::_1))
, _onUnloadProjectSlot(std::bind(&SceneWindow::OnUnloadProject, this, std::placeholders::_1))
{
	_ui->setupUi(this);

	_newShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_N), _ui->treeView);
	connect(_deleteShortcut, &QShortcut::activated, this, &SceneWindow::NewItem);

	_deleteShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), _ui->treeView);
	connect(_deleteShortcut, &QShortcut::activated, this, &SceneWindow::DeleteSelectedItems);

	_duplicateShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_D), _ui->treeView);
	connect(_duplicateShortcut, &QShortcut::activated, this, &SceneWindow::DuplicateSelectedItems);

	_sceneItemModel = new QStandardItemModel();
	_sceneItemModel->setColumnCount(1);

	_ui->treeView->setHeaderHidden(true);
	_ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
	_ui->treeView->setModel(_sceneItemModel);

	QObject::connect(_ui->treeView, &QTreeView::customContextMenuRequested, this, &SceneWindow::CustomMenuRequested);
	QObject::connect(_sceneItemModel, &QStandardItemModel::dataChanged, this, &SceneWindow::OnDataChanged);

	Project::GetInstance()->RegisterLoadProject(_onLoadProjectSlot);
	Project::GetInstance()->RegisterUnloadProject(_onUnloadProjectSlot);
}

///
///@brief Destroy the Scene:: Scene object
///
///
SceneWindow::~SceneWindow()
{
	Project::GetInstance()->UnregisterLoadProject(_onLoadProjectSlot);
	Project::GetInstance()->UnregisterUnloadProject(_onUnloadProjectSlot);

	delete _ui;
}

void SceneWindow::OpenSceneContent(SceneContent* sceneContent)
{
	_sceneContent = sceneContent;
}

///
///@brief 
///
///
void SceneWindow::OnLoadProject(Project* project)
{
	
}

///
///@brief 
///
///@param project 
///
void SceneWindow::OnUnloadProject(Project* project)
{
	_sceneItemModel->clear();
}

///
///@brief 
///
///@param topLeft 
///@param bottomRight 
///@param roles 
///
void SceneWindow::OnDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
	qDebug("Changed");
}

///
///@brief 
///
///@param position 
///
void SceneWindow::CustomMenuRequested(const QPoint& position)
{
	QMenu* menu = new QMenu(_ui->treeView);

	QModelIndexList indexList = _ui->treeView->selectionModel()->selectedIndexes();
	bool isMultiselection = (indexList.count() > 1);

	if (indexList.count() <= 1)
	{
		menu->addAction("New", this, &SceneWindow::NewItem, _newShortcut->key());

		if (indexList.count() == 1)
		{
			QModelIndex index = indexList[0];

			menu->addAction("Rename", [this, index]()
			{
				_ui->treeView->edit(index);
			}, QKeySequence(Qt::Key_F2));

			menu->addAction("Duplicate", this, &SceneWindow::DuplicateSelectedItems, _duplicateShortcut->key());
			menu->addAction("Delete", this, &SceneWindow::DeleteSelectedItems, _deleteShortcut->key());
		}
	}
	else // Multiselection
	{
		menu->addAction("Delete", this, &SceneWindow::DeleteSelectedItems, _deleteShortcut->key());
	}

	menu->popup(_ui->treeView->viewport()->mapToGlobal(position));
}

///
///@brief 
///
///
void SceneWindow::NewItem()
{
	//SceneTreeViewItem item = new SceneTreeViewItem();
	//_sceneItemModel->appendRow(item);
}

///
///@brief 
///
///
void SceneWindow::DeleteSelectedItems()
{
	if (QMessageBox::warning(this, "Delete Confirmation", "Do tou really want to delete this file(s) ?", QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::Cancel) == QMessageBox::StandardButton::Cancel)
	{
		return;
	}

	QModelIndexList indexList = _ui->treeView->selectionModel()->selectedIndexes();
	for (const QModelIndex& index : indexList)
	{
		SceneTreeViewItem* item = static_cast<SceneTreeViewItem*>(_sceneItemModel->itemFromIndex(index));
		_sceneItemModel->removeRow(item->row());
	}
}

///
///@brief 
///
///
void SceneWindow::DuplicateSelectedItems()
{
	/*
	QModelIndexList indexList = _ui->treeView->selectionModel()->selectedIndexes();
	if (indexList.size() == 0)
	{
		return;
	}

	ContentTreeViewItem* contentTreeViewItem = static_cast<ContentTreeViewItem*>(_sceneItemModel->itemFromIndex(indexList[0]));
	if (contentTreeViewItem->GetType() == FolderItem::_type)
	{
		QDir dstDir = static_cast<FolderItem*>(contentTreeViewItem)->GetDir();
		dstDir.cdUp();
		QDir newDir = DuplicateFolder(static_cast<FolderItem*>(contentTreeViewItem)->GetDir(), dstDir);

		FolderItem* newItem = new FolderItem(newDir);
		if (contentTreeViewItem->parent() != nullptr)
		{
			contentTreeViewItem->parent()->appendRow(newItem);
		}
		else
		{
			_sceneItemModel->invisibleRootItem()->appendRow(newItem);
		}

		FetchChildItem(newItem);

		_ui->treeView->edit(_sceneItemModel->indexFromItem(newItem));
	}
	else
	{
		Content* content = DuplicateContent(ItemToDir(contentTreeViewItem), static_cast<ContentItem*>(contentTreeViewItem)->GetContent());

		ContentItem* newItem = new ContentItem(content);
		if (contentTreeViewItem->parent() != nullptr)
		{
			contentTreeViewItem->parent()->appendRow(newItem);
		}
		else
		{
			_sceneItemModel->invisibleRootItem()->appendRow(newItem);
		}

		_ui->treeView->edit(_sceneItemModel->indexFromItem(newItem));
	}
	*/
}
