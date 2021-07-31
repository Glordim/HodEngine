
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
void Contents::OnDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
	qDebug("Changed");
}

///
///@brief 
///
///@param position 
///
void Contents::CustomMenuRequested(const QPoint& position)
{
	QMenu* menu = new QMenu(_ui->treeView);
	QModelIndexList indexList = _ui->treeView->selectionModel()->selectedIndexes();
	if (indexList.empty() == false)
	{
		if (indexList.count() == 1)
		{
			QModelIndex index = indexList[0];

			menu->addAction("Rename", [this, index]()
			{
				_ui->treeView->edit(index);
			});
		}

		menu->addAction("Delete", [this]()
		{
			
		});
	}
	else
	{
		QMenu* create = menu->addMenu("Create");
		create->addAction("Folder", [this]()
		{
			ContentDataBase::GetInstance()->CreateFolder();
		});
		QMenu* import = menu->addMenu("Import");
		import->addAction("Texture", [this]()
		{
			QString textureFilePath = QFileDialog::getOpenFileName(this, "Select a Texture file", Project::GetInstance()->GetAssetsFolderPath(), "*.png");

			ContentDataBase::GetInstance()->Import<TextureContent>(textureFilePath);
		});
		/*
		bool isFolder = false;
	if (index.isValid() == true)
	{
		QVariant data = _ui->treeView->model->data(index);
		data.
	}

	//QModelIndex index = _ui->treeView->indexAt(position);
	QStandardItem* item = nullptr;
	HODStandardItem* castItem = nullptr;

	QMenu* create = menu->addMenu("Create");
	create->addAction("Folder", [this]()
	{
			
	})

	if (index.isValid() == true)
	{
		item = _treeViewModel->itemFromIndex(index);
		castItem = dynamic_cast<HODStandardItem*>(item);

		if (castItem != nullptr)
		{
			if (castItem->GetType() == HODContentStandardItem::_type)
			{
				HODContentStandardItem* contentItem = dynamic_cast<HODContentStandardItem*>(item);

				if (contentItem != nullptr)
				{
					
					menu->addAction("Delete Content", [content = contentItem->GetContent()]()
						{
							ContentDataBase::GetInstance()->RemoveContent(content);
								//ContentDataBase::GetInstance()->GetContent()
						});
					menu->addAction("Move To", [this]()
						{

						});
				}

			}
			else // classe dossier a faire
			{
				menu->addAction("Delete Folder", [this]()
					{
						//ContentDataBase::GetInstance()->GetContent()
					});
				menu->addAction("Move To", [this]()
					{

					});
			}
		}
	}
	else
	{
		QMenu* create = menu->addMenu("Create");
		create->addAction("Folder", [this]()
		{
			
		})
		QMenu* import = menu->addMenu("Import");
		import->addAction("Texture", [this]()
		{
			QString textureFilePath = QFileDialog::getOpenFileName(this, "Select a Texture file", Project::GetInstance()->GetAssetsFolderPath(), "*.png");

			ContentDataBase::GetInstance()->Import<TextureContent>(textureFilePath);
		});
	}
	*/
	}

	menu->popup(_ui->treeView->viewport()->mapToGlobal(position));
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
