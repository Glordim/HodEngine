
#include "Contents.h"
#include "./ui_Contents.h"
#include <windows.h>

#include "ContentTreeViewItem.h"
#include "Items/ContentItem.h"
#include "Items/FolderItem.h"

#include "../../ContentDatabase.h"
#include "../../Contents/TextureContent.h"


#include <QMenu>
#include <QFileDialog>
#include <QTreeWidgetItem>

#include <QListView>
#include <QTableView>
#include <QTreeView>
#include <QStringList>
#include <String>

#include <QDesktopServices>

///
///@brief Construct a new Contents:: Contents object
///
///@param parent 
///
Contents::Contents(QWidget* parent)
: QDockWidget(parent)
, _ui(new Ui::Contents)
, _onAddContentSlot(std::bind(&Contents::OnAddContent, this, std::placeholders::_1))
, _onRemoveContentSlot(std::bind(&Contents::OnRemoveContent, this, std::placeholders::_1))
, _onContentChangeSlot(std::bind(&Contents::OnContentChange, this, std::placeholders::_1))
, _onLoadProjectSlot(std::bind(&Contents::OnLoadProject, this, std::placeholders::_1))
, _onUnloadProjectSlot(std::bind(&Contents::OnUnloadProject, this, std::placeholders::_1))
{
	_ui->setupUi(this);

	_contentItemModel = new QStandardItemModel();
	_contentItemModel->setColumnCount(1);

	_ui->treeView->setHeaderHidden(true);
	_ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
	_ui->treeView->setModel(_contentItemModel);

	QObject::connect(_ui->treeView, &QTreeView::customContextMenuRequested, this, &Contents::CustomMenuRequested);
	QObject::connect(_ui->treeView, &QTreeView::expanded, this, &Contents::OnExpandFolderItem);
	QObject::connect(_contentItemModel, &QStandardItemModel::dataChanged, this, &Contents::OnDataChanged);

	ContentDataBase::GetInstance()->GetAddContentSignal().Connect(_onAddContentSlot);
	ContentDataBase::GetInstance()->GetRemoveContentSignal().Connect(_onRemoveContentSlot);
	ContentDataBase::GetInstance()->GetContentChangeSignal().Connect(_onContentChangeSlot);

	Project::GetInstance()->RegisterLoadProject(_onLoadProjectSlot);
	Project::GetInstance()->RegisterUnloadProject(_onUnloadProjectSlot);
}

///
///@brief Destroy the Contents:: Contents object
///
///
Contents::~Contents()
{
	Project::GetInstance()->UnregisterLoadProject(_onLoadProjectSlot);
	Project::GetInstance()->UnregisterUnloadProject(_onUnloadProjectSlot);

	delete _ui;
}

///
///@brief 
///
///
void Contents::OnLoadProject(Project* project)
{
	FetchChildItem(_contentItemModel->invisibleRootItem());
}

///
///@brief 
///
///@param project 
///
void Contents::OnUnloadProject(Project* project)
{
	_contentItemModel->clear();
}

///
///@brief 
///
///@param topLeft 
///@param bottomRight 
///@param roles 
///
void Contents::OnDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
	qDebug("Changed");

	ContentTreeViewItem* item = static_cast<ContentTreeViewItem*>(_contentItemModel->itemFromIndex(topLeft));
	if (item->GetType() == FolderItem::_type)
	{
		FolderItem* folderItem = static_cast<FolderItem*>(item);
		folderItem->RenameDirWithCurrentText();
	}
	else
	{

	}
}

///
///@brief 
///
///@param position 
///
void Contents::CustomMenuRequested(const QPoint& position)
{
	QMenu* menu = new QMenu(_ui->treeView);

	bool isFolder = false;
	bool isMultiselection = false;
	QStandardItem* item = nullptr;

	QModelIndexList indexList = _ui->treeView->selectionModel()->selectedIndexes();
	if (indexList.empty() == true)
	{
		isFolder = true;
		item = _contentItemModel->invisibleRootItem();
	}
	else if (indexList.count() == 1)
	{
		item = _contentItemModel->itemFromIndex(indexList[0]);
		if (static_cast<ContentTreeViewItem*>(item)->GetType() == FolderItem::_type)
		{
			isFolder = true;
		}
	}
	else
	{
		isMultiselection = true;
	}

	if (isFolder == true)
	{
		AddMenuCreate(menu, item);
		menu->addSeparator();
	}
	if (isMultiselection == false)
	{
		menu->addAction("Show in Explorer", [this, item]()
		{
			ShowInExplorer(item);
		});
	}
	if (item != _contentItemModel->invisibleRootItem())
	{
		if (isMultiselection == false)
		{
			menu->addAction("Rename", [this, item]()
			{
				_ui->treeView->edit(_contentItemModel->indexFromItem(item));
			}, QKeySequence(Qt::Key_F2));

			menu->addAction("Duplicate", [this, item]()
			{
				ContentTreeViewItem* contentTreeViewItem = static_cast<ContentTreeViewItem*>(item);
				if (contentTreeViewItem->GetType() == FolderItem::_type)
				{
					QDir dstDir = static_cast<FolderItem*>(contentTreeViewItem)->GetDir();
					dstDir.cdUp();
					DuplicateFolder(static_cast<FolderItem*>(contentTreeViewItem)->GetDir(), dstDir);
				}
				else
				{
					//ContentDatabase::GetInstance()->DuplicateContent(static_cast<ContentItem*>(item)->GetContent());
				}

				OnUnloadProject(nullptr);
				OnLoadProject(nullptr);
			}, QKeySequence(Qt::CTRL | Qt::Key_D));
		}
		menu->addAction("Delete", [this, indexList]()
		{
			for (const QModelIndex& index : indexList)
			{
				ContentTreeViewItem* item = static_cast<ContentTreeViewItem*>(_contentItemModel->itemFromIndex(index));
				if (item->GetType() == FolderItem::_type)
				{
					QDir dir = static_cast<FolderItem*>(item)->GetDir();
					dir.removeRecursively();
				}
				else
				{
					QFile file(static_cast<ContentItem*>(item)->GetContent()->GetPath());
					file.remove();
				}

				OnUnloadProject(nullptr);
				OnLoadProject(nullptr);
			}
		}, QKeySequence(Qt::Key_Delete));
	}

	menu->popup(_ui->treeView->viewport()->mapToGlobal(position));
}

///
///@brief 
///
///@param menu 
///@param item 
///
void Contents::AddMenuCreate(QMenu* menu, QStandardItem* item)
{
	QMenu* create = menu->addMenu("Create");
	create->addAction("Folder", [this, item]()
	{
		QDir dir = ItemToDir(item);
		dir.mkdir("Folder");
		dir.cd("Folder");

		FolderItem* newItem = new FolderItem(dir);
		item->appendRow(newItem);

		_ui->treeView->edit(_contentItemModel->indexFromItem(newItem));
	});
	create->addSeparator();
	create->addAction("Texture", [this]()
	{
		QString textureFilePath = QFileDialog::getOpenFileName(this, "Select a Texture file", Project::GetInstance()->GetAssetsFolderPath(), "*.png");

		ContentDataBase::GetInstance()->Import<TextureContent>(textureFilePath);
	});
}

///
///@brief Open a native Explore window
///
///@param item TreeView to show in Explorer
///
void Contents::ShowInExplorer(QStandardItem* item)
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(ItemToDir(item).absolutePath()));
}

///
///@brief 
///
///@param dir 
///
void Contents::DuplicateFolder(const QDir& srcDir, const QDir& dstDir)
{
	dstDir.mkdir(srcDir.dirName() + " (copy)");
	QDir newDir(dstDir.path() + "/" + srcDir.dirName() + " (copy)");

	QFileInfoList fileInfoList = srcDir.entryInfoList(QDir::Filter::Dirs | QDir::Filter::Files | QDir::NoDotAndDotDot);

	for (const QFileInfo& fileInfo : fileInfoList)
	{
		if (fileInfo.isDir() == true)
		{
			DuplicateFolder(fileInfo.dir(), newDir);
		}
		else if (fileInfo.isFile() == true)
		{
			if (fileInfo.suffix() == "content")
			{
				//ContentDataBase::GetInstance()->DuplicateContent(fileInfo.absoluteFilePath());
			}
		}
	}
}

///
///@brief 
///
///@param item 
///@return QDir 
///
QDir Contents::ItemToDir(QStandardItem* item)
{
	QDir dir;

	if (item == _contentItemModel->invisibleRootItem())
	{
		dir.setPath(Project::GetInstance()->GetContentsFolderPath());
	}
	else
	{
		ContentTreeViewItem* contentTreeViewItem = static_cast<ContentTreeViewItem*>(item);
		if (contentTreeViewItem->GetType() == FolderItem::_type)
		{
			dir = static_cast<FolderItem*>(contentTreeViewItem)->GetDir();
		}
		else
		{
			QFileInfo fileInfo(static_cast<ContentItem*>(contentTreeViewItem)->GetContent()->GetPath());
			dir = fileInfo.dir();
		}
	}

	return dir;
}

///
///@brief 
///
///@param index 
///
void Contents::OnExpandFolderItem(const QModelIndex& index)
{
	//FetchChildItem(_contentItemModel->itemFromIndex(index));
}

///
///@brief 
///
///@param index 
///
void Contents::FetchChildItem(QStandardItem* parentItem)
{
	QDir dir;

	if (parentItem != _contentItemModel->invisibleRootItem())
	{
		dir = static_cast<FolderItem*>(parentItem)->GetDir();
	}
	else
	{
		QString contentsRootPath = Project::GetInstance()->GetContentsFolderPath();
		dir.setPath(contentsRootPath);
	}

	QFileInfoList fileInfoList = dir.entryInfoList(QDir::Filter::Dirs | QDir::Filter::Files | QDir::NoDotAndDotDot, QDir::SortFlag::Name | QDir::SortFlag::DirsFirst);

	for (const QFileInfo& fileInfo : fileInfoList)
	{
		if (fileInfo.isDir() == true)
		{
			FolderItem* newItem = new FolderItem(QDir(fileInfo.absoluteFilePath()));
			parentItem->appendRow(newItem);

			FetchChildItem(newItem);
		}
		else if (fileInfo.isFile() == true)
		{
			if (fileInfo.suffix() == "content")
			{
				Content* content = ContentDataBase::GetInstance()->GetContent(fileInfo.absoluteFilePath());

				ContentItem* newItem = new ContentItem(content);
				parentItem->appendRow(newItem);
			}
		}
	}
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Contents::OnAddContent(Content* content)
{
	/*
	ContentTreeViewItem* parentItem = FindParentItem(content);

	if (parentItem != nullptr)
	{
		ContentItem* item = new ContentItem(content);
		parentItem->appendRow(item);
	}
	*/
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Contents::OnRemoveContent(Content* content)
{
	/*
	const QString& contentPath = content->GetPath();
	qsizetype offset = contentPath.indexOf("/Contents/");
	if (offset == -1)
	{
		qWarning("Contents::OnAddContent::Content out of folder Contents");
		return;
	}

	QStringList splitPath = contentPath.mid(offset + strlen("/Contents/")).split('/');

	QStandardItem* folder = _contentItemModel->invisibleRootItem();

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
			return;
		}

		folder = childFolder;
	}

	int childCount = folder->rowCount();
	const QString& contentName = content->GetName();

	for (qsizetype childIndex = 0; childIndex < childCount; ++childIndex)
	{
		if (folder->child(childIndex)->text() == contentName)
		{
			folder->removeRow(childIndex);
			return;
		}
	}
	*/
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Contents::OnContentChange(Content* content)
{
	// refresh view
}

///
///@brief 
///
///@param content 
///@return ContentTreeViewItem* 
///
ContentTreeViewItem* Contents::FindParentItem(Content* content)
{
	ContentTreeViewItem* parentItem = static_cast<ContentTreeViewItem*>(_contentItemModel->invisibleRootItem());

	const QString& contentPath = content->GetPath();

	QString contentsRootFolderPath = Project::GetInstance()->GetContentsFolderPath();
	if (contentPath.startsWith(contentsRootFolderPath) == false)
	{
		qWarning("Contents::FindParentItem: Content out of folder \"Contents\"");
		return nullptr;
	}

	QStringList splitPath = contentPath.mid(contentsRootFolderPath.size() + 1).split('/');

	qsizetype splitPathCount = splitPath.length();
	for (qsizetype splitPathIndex = 0; splitPathIndex < splitPathCount - 1; ++splitPathIndex) // -1 to iterate only on intermediate folders
	{
		const QString& name = splitPath.at(splitPathIndex);

		ContentTreeViewItem* childFolder = nullptr;

		int childCount = parentItem->rowCount();
		for (int childIndex = 0; childIndex < childCount; ++childIndex)
		{
			ContentTreeViewItem* child = static_cast<ContentTreeViewItem*>(parentItem->child(childIndex));
			if (child->text() == name)
			{
				childFolder = child;
				break;
			}
		}

		if (childFolder == nullptr)
		{
			qWarning("Contents::FindParentItem: missing intermediate folder");
			return nullptr;
		}

		parentItem = childFolder;
	}

	return parentItem;
}
