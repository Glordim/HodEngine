#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class NewProjectDialog;
}
QT_END_NAMESPACE

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class NewProjectDialog : public QDialog
{
	Q_OBJECT

public:

							NewProjectDialog(QWidget* parent = nullptr);
							NewProjectDialog(const NewProjectDialog&) = delete;
							NewProjectDialog(NewProjectDialog&&) = delete;
							~NewProjectDialog() override;

	void					operator=(const NewProjectDialog&) = delete;
	void					operator=(NewProjectDialog&&) = delete;

public Q_SLOTS:

	void					accept() override;

private:

	void					OpenFileBrowser();

private:

	Ui::NewProjectDialog*	_ui;
};

