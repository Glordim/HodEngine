#pragma once

#include <QDockWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class SceneWindow;
}
QT_END_NAMESPACE

#include <QDir>

#include "../DockableWindow.h"
#include "../../Project.h"

class QMenu;
class QShortcut;
class QStandardItem;
class FolderItem;
class ContentItem;
class ContentTreeViewItem;

class SceneContent;
class SceneModel;

class QItemSelection;

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class SceneWindow : public DockableWindow
{
	Q_OBJECT

	META_TYPE(SceneWindow)

public:

												SceneWindow(QWidget* parent = nullptr);
												~SceneWindow();

	void										OpenSceneContent(SceneContent* sceneContent);
	void										CloseSceneContent();

private:

	void										CustomMenuRequested(const QPoint& position);

	void										OnDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

public slots:

	void										SelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

	void										NewEntity();
	void										DeleteSelectedItems();
	void										DuplicateSelectedItems();

private:

	Ui::SceneWindow*							_ui;
	SceneModel*									_sceneModel;

	QShortcut*									_newShortcut = nullptr;
	QShortcut*									_deleteShortcut = nullptr;
	QShortcut*									_duplicateShortcut = nullptr;
};
