#pragma once

#include <QDialog>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class ComponentLibraryDialog;
}
QT_END_NAMESPACE

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class ComponentLibraryDialog : public QDialog
{
	Q_OBJECT

public:

							ComponentLibraryDialog(QWidget* parent = nullptr);
							ComponentLibraryDialog(const ComponentLibraryDialog&) = delete;
							ComponentLibraryDialog(ComponentLibraryDialog&&) = delete;
							~ComponentLibraryDialog() override;

	void					operator=(const ComponentLibraryDialog&) = delete;
	void					operator=(ComponentLibraryDialog&&) = delete;

signals:

	void					OnAcceptComponent(const QString& componentTypeName);

public slots:

	void					accept() override;
	void					OnValidate(const QModelIndex& index);

private:

	Ui::ComponentLibraryDialog*	_ui;

	QStandardItemModel*			_model;
};

