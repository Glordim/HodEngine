#include "NewProjectDialog.h"
#include "./ui_NewProjectDialog.h"

#include "../Project.h"
#include "../Mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
NewProjectDialog::NewProjectDialog(QWidget *parent)
	: QDialog(parent)
	, _ui(new Ui::NewProjectDialog)
{
	_ui->setupUi(this);

	QObject::connect(_ui->formLocationBrowseButton, &QToolButton::clicked, this, &NewProjectDialog::OpenFileBrowser);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
NewProjectDialog::~NewProjectDialog()
{
	delete _ui;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void NewProjectDialog::OpenFileBrowser()
{
	_ui->formLocationLineEdit->setText(QFileDialog::getExistingDirectory(this));
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void NewProjectDialog::accept()
{
	QString name = _ui->formNameLineEdit->text();
	if (name.isEmpty() == true)
	{
		QMessageBox::critical(this, "New Project", "Please enter a valid project Name");
		return;
	}

	QString location = _ui->formLocationLineEdit->text();
	if (location.isEmpty() == true)
	{
		QMessageBox::critical(this, "New Project", "Please enter a valid project Location");
		return;
	}

	if (Project::CreateOnDisk(name, location) == true)
	{
		close();

		QString projectFilePath = location + "/" + name + ".hod";

		// Hum parent ? Prefer a MainWindow Singleton
		static_cast<MainWindow*>(parentWidget())->LoadProjectAtPath(projectFilePath);
	}
}
