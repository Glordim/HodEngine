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
							~NewProjectDialog();

private:

	void					OpenFileBrowser();
	bool					CreateProject();

private:

	Ui::NewProjectDialog*	_ui;
};

