#pragma once

#include <QDockWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class SceneWindow;
}
QT_END_NAMESPACE

#include <QStandardItemModel>
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

private:

	void										CustomMenuRequested(const QPoint& position);

	void										OnLoadProject(Project* project);
	void										OnUnloadProject(Project* project);

	void										OnDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

	void										CreateEntity();

public slots:

	void										NewItem();
	void										DeleteSelectedItems();
	void										DuplicateSelectedItems();

private:

	Ui::SceneWindow*							_ui;
	QStandardItemModel*							_sceneItemModel;

	QShortcut*									_newShortcut = nullptr;
	QShortcut*									_deleteShortcut = nullptr;
	QShortcut*									_duplicateShortcut = nullptr;

	Project::LoadProjectSignal::Slot			_onLoadProjectSlot;
	Project::UnloadProjectSignal::Slot			_onUnloadProjectSlot;

	SceneContent*								_sceneContent = nullptr;
};
