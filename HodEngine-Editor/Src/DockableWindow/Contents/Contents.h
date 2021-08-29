#pragma once

#include <QDockWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class Contents;
}
QT_END_NAMESPACE

#include "../../ContentDatabase.h"
#include <QStandardItemModel>
#include <QDir>

class QMenu;
class QStandardItem;
class FolderItem;
class ContentItem;
class ContentTreeViewItem;

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class Contents : public QDockWidget
{
	Q_OBJECT

public:

												Contents(QWidget* parent = nullptr);
												~Contents();

private:

	void										CustomMenuRequested(const QPoint& position);
	void										AddMenuCreate(QMenu* menu, QStandardItem* item);
	void										ShowInExplorer(QStandardItem* item);
	void										DuplicateFolder(const QDir& srcDir, const QDir& dstDir);

	QDir										ItemToDir(QStandardItem* item);

	void										OnExpandFolderItem(const QModelIndex& index);
	void										FetchChildItem(QStandardItem* parentItem);

	void										OnAddContent(Content* content);
	void										OnRemoveContent(Content* content);
	void										OnContentChange(Content* content);

	void										OnLoadProject(Project* project);
	void										OnUnloadProject(Project* project);

	void										OnDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

	ContentTreeViewItem*						FindParentItem(Content* content);

	QDir										CreateFolder(const QDir& dir, const QString& name);
	void										DeleteFolder(FolderItem* folderItem);

	bool										CreateContent(const QDir& parentDir, Content* content);
	void										DeleteContent(ContentItem* contentItem);

private:

	Ui::Contents*								_ui;
	QStandardItemModel*							_contentItemModel;


	ContentDataBase::AddContentSignal::Slot		_onAddContentSlot;
	ContentDataBase::RemoveContentSignal::Slot	_onRemoveContentSlot;
	ContentDataBase::ContentChangeSignal::Slot	_onContentChangeSlot;

	Project::LoadProjectSignal::Slot			_onLoadProjectSlot;
	Project::UnloadProjectSignal::Slot			_onUnloadProjectSlot;
};
