#pragma once

#include <QDockWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class ContentWindow;
}
QT_END_NAMESPACE

#include "../../ContentDatabase.h"
#include <QStandardItemModel>
#include <QDir>

#include "../DockableWindow.h"

class QMenu;
class QShortcut;
class QStandardItem;
class FolderItem;
class ContentItem;
class ContentTreeViewItem;

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class ContentWindow : public DockableWindow
{
	Q_OBJECT

	META_TYPE(ContentWindow)

public:

												ContentWindow(QWidget* parent = nullptr);
												~ContentWindow();

private:

	void										CustomMenuRequested(const QPoint& position);
	void										AddMenuCreate(QMenu* menu, QStandardItem* item);
	void										ShowInExplorer(QStandardItem* item);

	void										OnDoubleClick(const QModelIndex& index);

	QDir										ItemToDir(QStandardItem* item);

	void										OnExpandFolderItem(const QModelIndex& index);
	void										FetchChildItem(QStandardItem* parentItem);

	void										OnAddContent(Content* content);
	void										OnRemoveContent(Content* content);
	void										OnContentChange(Content* content);

	void										OnDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

	ContentTreeViewItem*						FindParentItem(Content* content);

	QString										FindNextAvailableFilePath(const QDir& dir, const QString& name, const QString& extension);

	QDir										CreateFolder(const QDir& dir, const QString& name);
	QDir										DuplicateFolder(const QDir& srcDir, const QDir& dstDir);
	void										DeleteFolder(FolderItem* folderItem);

	bool										CreateContent(const QDir& parentDir, Content* content);
	Content*									DuplicateContent(const QDir& dir, Content* content);
	void										DeleteContent(ContentItem* contentItem);

public slots:

	void										OnLoadProject();
	void										OnUnloadProject();

	void										DeleteSelectedItems();
	void										DuplicateSelectedItems();

private:

	Ui::ContentWindow*							_ui;
	QStandardItemModel*							_contentItemModel;

	QShortcut*									_deleteShortcut = nullptr;
	QShortcut*									_duplicateShortcut = nullptr;

	ContentDataBase::AddContentSignal::Slot		_onAddContentSlot;
	ContentDataBase::RemoveContentSignal::Slot	_onRemoveContentSlot;
	ContentDataBase::ContentChangeSignal::Slot	_onContentChangeSlot;
};
