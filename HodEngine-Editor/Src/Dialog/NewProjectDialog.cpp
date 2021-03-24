#include "NewProjectDialog.h"
#include "./ui_NewProjectDialog.h"

#include "../Project.h"
#include "../Mainwindow.h"

#include <QFileDialog>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
NewProjectDialog::NewProjectDialog(QWidget *parent)
	: QDialog(parent)
	, _ui(new Ui::NewProjectDialog)
{
	_ui->setupUi(this);

	QObject::connect(_ui->pushButton, &QPushButton::clicked, this, &NewProjectDialog::OpenFileBrowser);
	QObject::connect(_ui->okButton, &QPushButton::clicked, this, &NewProjectDialog::CreateProject);
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
	_ui->lineEdit_2->setText(QFileDialog::getExistingDirectory(this));
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool NewProjectDialog::CreateProject()
{
	Project project;
	project.SetName(_ui->lineEdit->text());
	project.SaveAtPath(_ui->lineEdit_2->text() + "/" + project.GetName() + ".hod");
	QDir(_ui->lineEdit_2->text()).mkdir("Contents");

	// Hum parent ? Prefer a MainWindow Singleton
	if (static_cast<MainWindow*>(parentWidget())->LoadProjectAtPath(project.GetSavePath()) == false)
	{
		return false;
	}

	close();
	return true;
}
