#pragma once

#include <QDockWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class EntityWindow;
}
QT_END_NAMESPACE

#include <QStandardItemModel>
#include <QDir>

#include "../DockableWindow.h"
#include "../../Project.h"

#include "../../Contents/SceneContent.h"

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
class EntityWindow : public DockableWindow
{
	Q_OBJECT

	META_TYPE(EntityWindow)

public:

						EntityWindow(QWidget* parent = nullptr);
						~EntityWindow();

	void				OpenEntity(SceneContent::Entity* entity);
	void				CloseEntity();

private:

	void				CustomMenuRequested(const QPoint& position);

public slots:

	void				OnAddComponent();
	void				OnAddComponentResult(const QString& componentTypeName);

private:

	Ui::EntityWindow*	_ui;

	QShortcut*			_newShortcut = nullptr;

	SceneContent::Entity*	_entity;
};
