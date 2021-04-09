
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

	_ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
	_treeViewModel = new QStandardItemModel(_ui->treeView);
	_ui->treeView->setModel(_treeViewModel);
	_treeViewModel->setColumnCount(1);
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
	QMenu* create = menu->addMenu("Create");
	QMenu* import = menu->addMenu("Import");
	import->addAction("Texture", [this]()
		{
			QString textureFilePath = QFileDialog::getOpenFileName(this, "Select a Texture file", Project::GetInstance()->GetAssetsFolderPath(), "*.png");

			ContentDataBase::GetInstance()->Import<TextureContent>(textureFilePath);
		});

	menu->popup(_ui->treeView->viewport()->mapToGlobal(position));
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Contents::OnAddContent(Content* content)
{
	QStringList splitPath = content->GetPath().split("/");

	QStandardItem* parentFolder = nullptr;
	QStandardItem* item = nullptr;

	qsizetype contentFolderIndex = -1;
	bool isFolderFind;

	// Recherche de l'index du dossier content
	for (qsizetype i = 0; i < splitPath.length(); ++i)
	{
		if (splitPath.value(i) == "Contents")
		{
			contentFolderIndex = i;
			break;
		}
	}

	if (contentFolderIndex == -1)
	{
		qWarning("Contents::OnAddContent::Content out of folder Contents");
		return;
	}

	parentFolder = _treeViewModel->invisibleRootItem();

	for (qsizetype i = 1; contentFolderIndex + i < splitPath.length(); ++i)
	{
		isFolderFind = false;
		const QString name = splitPath.value(contentFolderIndex + i);


		// Fin de chaine
		if (contentFolderIndex + i == splitPath.length() - 1)
		{
			item = new QStandardItem(name);
			if (parentFolder != nullptr)
			{
				parentFolder->appendRow(item);
			}
			else	// Fichier à la racine de Content
			{
				_treeViewModel->appendRow(item);
			}
			return;
		}

		if (parentFolder != nullptr)
		{
			for (int i = 0; i < parentFolder->rowCount(); ++i)
			{
				for (int j = 0; j < parentFolder->columnCount(); ++j)
				{
					if (parentFolder->child(i, j)->text() == name)
					{
						parentFolder = parentFolder->child(i, j);
						isFolderFind = true;
						break;
						break;
					}
				}
			}
		}

		if (isFolderFind == false)
		{
			item = new QStandardItem(name);
			if (parentFolder != nullptr)
			{
				parentFolder->appendRow(item);
			}
			else
			{
				_treeViewModel->appendRow(item);
			}
			parentFolder = item;
		}
	}
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
