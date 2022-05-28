#include "ComponentLibraryDialog.h"
#include "./ui_ComponentLibraryDialog.h"

#include "../../ApplicationDescription.h"

#include <QFile>
#include <QStandardItem>

/// @brief 
/// @param parent 
ComponentLibraryDialog::ComponentLibraryDialog(QWidget *parent)
	: QDialog(parent)
	, _ui(new Ui::ComponentLibraryDialog)
{
	_ui->setupUi(this);

	_model = new QStandardItemModel(0, 2, this);

	_ui->treeView->setModel(_model);
	_ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);

	_model->setHeaderData(0, Qt::Horizontal, "Component", Qt::DisplayRole);
	_model->setHeaderData(1, Qt::Horizontal, "Parent", Qt::DisplayRole);
	_ui->treeView->header()->setSectionResizeMode(QHeaderView::Stretch);
	
	QObject::connect(_ui->treeView, &QTreeView::doubleClicked, this, &ComponentLibraryDialog::accept);
	QObject::connect(_ui->treeView, &QTreeView::entered, this, &ComponentLibraryDialog::accept);

	QJsonArray componentsNode = ApplicationDescription::GetInstance()->GetComponentsArray();
	QJsonArray::const_iterator it = componentsNode.cbegin();
	QJsonArray::const_iterator itEnd = componentsNode.cend();
	while (it != itEnd)
	{
		QList<QStandardItem*> rowItems;

		QJsonObject component = it->toObject();
		QString name = component["TypeName"].toString();
		QString parentType = component["ParentTypeName"].toString();

		QStandardItem* item = new QStandardItem();
		item->setData(name, Qt::DisplayRole);
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);

		QStandardItem* item2 = new QStandardItem();
		item2->setData(parentType, Qt::DisplayRole);
		item2->setFlags(item->flags() & ~Qt::ItemIsEditable);

		rowItems.append(item);
		rowItems.append(item2);

		_model->invisibleRootItem()->appendRow(rowItems);

		++it;
	}
}

/// @brief 
ComponentLibraryDialog::~ComponentLibraryDialog()
{
	delete _ui;
}

/// @brief 
void ComponentLibraryDialog::accept()
{
	QModelIndexList selection = _ui->treeView->selectionModel()->selectedRows();
	if (selection.isEmpty() == false)
	{
		OnValidate(selection[0]);
	}
}

void ComponentLibraryDialog::OnValidate(const QModelIndex& index)
{
	QString componentTypeName = _model->takeRow(index.row())[0]->text();

	emit OnAcceptComponent(componentTypeName);

	close();
}
