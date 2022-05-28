
#include "SceneWindow.h"
#include "./ui_SceneWindow.h"
#include <windows.h>

#include "../EntityWindow/EntityWindow.h"
#include "../../mainwindow.h"

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

#include "../../Contents/SceneContent.h"

#include "SceneModel.h"
#include "SceneItem.h"
#include "EntityItem.h"

#include "../../mainwindow.h"
#include "../../DockableWindow/SceneWindow/SceneWindow.h"
#include "../../DockableWindow/ViewportWindow.h"

/// @brief 
/// @param parent 
SceneWindow::SceneWindow(QWidget* parent)
: DockableWindow(parent)
, _ui(new Ui::SceneWindow)
{
	_ui->setupUi(this);

	_newShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_N), _ui->treeView);
	connect(_deleteShortcut, &QShortcut::activated, this, &SceneWindow::NewEntity);

	_deleteShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), _ui->treeView);
	connect(_deleteShortcut, &QShortcut::activated, this, &SceneWindow::DeleteSelectedItems);

	_duplicateShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_D), _ui->treeView);
	connect(_duplicateShortcut, &QShortcut::activated, this, &SceneWindow::DuplicateSelectedItems);

	_sceneModel = new SceneModel();
	_sceneModel->setColumnCount(1);

	_ui->treeView->setHeaderHidden(true);
	_ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
	_ui->treeView->setModel(_sceneModel);

	QObject::connect(_ui->treeView, &QTreeView::customContextMenuRequested, this, &SceneWindow::CustomMenuRequested);
	QObject::connect(_sceneModel, &QStandardItemModel::dataChanged, this, &SceneWindow::OnDataChanged);
	QObject::connect(_ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &SceneWindow::SelectionChanged);

	QObject::connect(Project::GetInstance(), &Project::Unloaded, this, &SceneWindow::CloseSceneContent);

	setEnabled(false);
}

///
///@brief Destroy the Scene:: Scene object
///
///
SceneWindow::~SceneWindow()
{
	delete _ui;
}

void SceneWindow::OpenSceneContent(SceneContent* sceneContent)
{
	CloseSceneContent();

	SceneItem* sceneItem = new SceneItem(sceneContent);
	_sceneModel->invisibleRootItem()->appendRow(sceneItem);

	QJsonObject sceneData;
	sceneContent->Serialize(sceneData);

	ViewportWindow* viewportWindow = MainWindow::GetInstance()->GetOrCreateDockableWindow<ViewportWindow>();
	viewportWindow->SendEngineRequest("AddEntity", sceneData);

	setEnabled(true);
}

/// @brief 
void SceneWindow::CloseSceneContent()
{
	_sceneModel->clear();

	ViewportWindow* viewportWindow = MainWindow::GetInstance()->GetOrCreateDockableWindow<ViewportWindow>();
	viewportWindow->SendEngineRequest("Reset");

	setEnabled(false);
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

void SceneWindow::SelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	EntityItem* entityItem = nullptr;

	if (selected.isEmpty() == false)
	{
		entityItem = dynamic_cast<EntityItem*>(_sceneModel->itemFromIndex(selected[0].indexes()[0]));
	}

	EntityWindow* entityWindow = MainWindow::GetInstance()->GetOrCreateDockableWindow<EntityWindow>();

	if (entityItem != nullptr)
	{
		entityWindow->OpenEntity(entityItem->GetEntity());
	}
	else
	{
		entityWindow->CloseEntity();
	}
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
		menu->addAction("New", this, &SceneWindow::NewEntity, _newShortcut->key());

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

/// @brief 
void SceneWindow::NewEntity()
{
	QModelIndexList indexList = _ui->treeView->selectionModel()->selectedIndexes();
	QModelIndex index = indexList[0];

	QStandardItem* parent = _sceneModel->itemFromIndex(index);
	while (parent->parent() != nullptr)
	{
		parent = parent->parent();
	}

	SceneItem* sceneItem = (SceneItem*)parent;
	SceneContent::Entity* entity = sceneItem->GetSceneContent()->CreateEntity();

	EntityItem* entityItem = new EntityItem(entity);
	entityItem->setText("Entity");

	parent = _sceneModel->itemFromIndex(index);
	if (parent->parent() != _sceneModel->invisibleRootItem())
	{
		//entity->_components = 
	}

	parent->appendRow(entityItem);

	_ui->treeView->setExpanded(index, true);

	_ui->treeView->selectionModel()->clear();
	_ui->treeView->selectionModel()->select(_sceneModel->indexFromItem(entityItem), QItemSelectionModel::Select);
	_ui->treeView->edit(_sceneModel->indexFromItem(entityItem));
}

/// @brief 
void SceneWindow::DeleteSelectedItems()
{
	if (QMessageBox::warning(this, "Delete Confirmation", "Do tou really want to delete this file(s) ?", QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::Cancel) == QMessageBox::StandardButton::Cancel)
	{
		return;
	}

	/*
	QModelIndexList indexList = _ui->treeView->selectionModel()->selectedIndexes();
	for (const QModelIndex& index : indexList)
	{
		SceneTreeViewItem* item = static_cast<SceneTreeViewItem*>(_sceneModel->itemFromIndex(index));
		_sceneModel->removeRow(item->row());
	}
	*/
}

/// @brief 
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
