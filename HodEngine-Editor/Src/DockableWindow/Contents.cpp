
#include "Contents.h"
#include "./ui_Contents.h"
#include "../Project.h"
#include <windows.h>

#include "HODContentStandardItem.h"
#include "HODDirectoryStandardItem.h"
#include "../ContentDatabase.h"
#include "../Contents/TextureContent.h"

#include <QMenu>
#include <QFileDialog>
#include <QTreeWidgetItem>
#include <QStandardItem>
#include <Qmessagebox>
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
	HODStandardItem* castItem = __nullptr;

	if (index.isValid())
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
			else if (castItem->GetType() == HODDirectoryStandardItem::_type)
			{
				HODDirectoryStandardItem* directotyItem = dynamic_cast<HODDirectoryStandardItem*>(item);

				if (directotyItem != nullptr)
				{
					menu->addAction("Folder", [this]()
						{
							//CreateContentDirectory(
						});
					menu->addAction("Delete Folder", [directory = directotyItem->GetDir()]()
						{
							directory->removeRecursively();
						});
					menu->addAction("Move To", [this]()
						{

						});
				}
			}
		}
	}
	else
	{
		QMenu* create = menu->addMenu("Create");
		menu->addAction("Folder", [this]()
			{		
				QStandardItem* folder = _treeViewModel->invisibleRootItem();
				QString path = Project::GetInstance()->GetContentsFolderPath();
				path += "/Folder";
				folder->appendRow(new HODDirectoryStandardItem(path));
			});

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

	HODContentStandardItem* item = new HODContentStandardItem(content);

	folder->appendRow(item);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Contents::OnRemoveContent(Content* content)
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

}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Contents::OnContentChange(Content* content)
{
	// refresh view
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Contents::CreateContentDirectory(QString& path)
{
	QDir dir(path);
	if (dir.exists() == true)
	{
		QMessageBox::critical(nullptr, "ContentDataBase", "Unable to create folder\n'" + dir.path() + "' folder exist");
		return;
	}

	dir.mkdir(path);
}
