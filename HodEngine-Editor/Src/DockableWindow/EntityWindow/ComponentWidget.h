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

#include "../../Widgets/Collaspsable.h"

class QMenu;
class QShortcut;
class QStandardItem;
class FolderItem;
class ContentItem;
class ContentTreeViewItem;

class SceneContent;

/// @brief 
class ComponentWidget : public Collaspsable
{
	Q_OBJECT

public:

								ComponentWidget(SceneContent::Component* component);
								~ComponentWidget();

private:

	void						CustomMenuRequested(const QPoint& position);

private:

	SceneContent::Component*	_component;

	QVBoxLayout*				_layout;
};
