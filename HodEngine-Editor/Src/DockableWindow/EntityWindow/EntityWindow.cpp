
#include "EntityWindow.h"
#include "./ui_EntityWindow.h"
#include <windows.h>

#include <QMenu>
#include <QFileDialog>
#include <QTreeWidgetItem>

#include <QPushButton>
#include <QListView>
#include <QTableView>
#include <QTreeView>
#include <QStringList>
#include <QString>
#include <QMessageBox>
#include <QShortcut>
#include <QDesktopServices>
#include <QScrollArea>

#include "../../Dialog/ComponentLibraryDialog/ComponentLibraryDialog.h"

#include <QVBoxLayout>
#include "ComponentWidget.h"

/// @brief 
/// @param parent 
EntityWindow::EntityWindow(QWidget* parent)
: DockableWindow(parent)
, _ui(new Ui::EntityWindow)
{
	_ui->setupUi(this);

	_newShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_N), _ui->ScrollArea);

	QObject::connect(_ui->addComponentButton, &QPushButton::clicked, this, &EntityWindow::OnAddComponent);
	QObject::connect(_ui->ScrollArea, &QScrollArea::customContextMenuRequested, this, &EntityWindow::CustomMenuRequested);

	QObject::connect(Project::GetInstance(), &Project::Unloaded, this, &EntityWindow::CloseEntity);

	setEnabled(false);
}

/// @brief 
EntityWindow::~EntityWindow()
{
	delete _ui;
}

void EntityWindow::OpenEntity(SceneContent::Entity* entity)
{
	_entity = entity;

	for (SceneContent::Component* component : _entity->_components)
	{
		ComponentWidget* componentWidget = new ComponentWidget(component);

		int index = _ui->verticalLayout_3->count() - 2;
		_ui->verticalLayout_3->insertWidget(index, componentWidget);
	}

	setEnabled(true);
}

void EntityWindow::CloseEntity()
{
	_entity = nullptr;

	while (_ui->verticalLayout_3->count() > 2)
	{
		QWidget* widget = _ui->verticalLayout_3->takeAt(0)->widget();
		_ui->verticalLayout_3->removeWidget(widget);
		delete widget;
	}

	setEnabled(false);
}

/// @brief 
/// @param position 
void EntityWindow::CustomMenuRequested(const QPoint& position)
{
	QMenu* menu = new QMenu(_ui->ScrollArea);

	menu->addAction("Add Component", this, &EntityWindow::OnAddComponent, _newShortcut->key());

	menu->popup(_ui->ScrollArea->viewport()->mapToGlobal(position));
}

/// @brief 
void EntityWindow::OnAddComponent()
{
	ComponentLibraryDialog* componentLibraryDialog = new ComponentLibraryDialog(this);
	QObject::connect(componentLibraryDialog, &ComponentLibraryDialog::OnAcceptComponent, this, &EntityWindow::OnAddComponentResult);
	componentLibraryDialog->show();
}

/// @brief 
void EntityWindow::OnAddComponentResult(const QString& componentTypeName)
{
	SceneContent::Component* component = _entity->AddComponent(componentTypeName);

	ComponentWidget* componentWidget = new ComponentWidget(component);

	int index = _ui->verticalLayout_3->count() - 2;
	_ui->verticalLayout_3->insertWidget(index, componentWidget);

	qDebug() << componentTypeName;
}

