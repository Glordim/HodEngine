#include "NewProjectDialog.h"
#include "./ui_NewProjectDialog.h"

#include "../Project.h"
#include "../Mainwindow.h"

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
NewProjectDialog::NewProjectDialog(QWidget *parent)
	: QDialog(parent)
	, _ui(new Ui::NewProjectDialog)
{
	_ui->setupUi(this);

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
bool NewProjectDialog::CreateProject()
{
	Project project;
	project.SetName(_ui->lineEdit->text());
	project.SaveAtPath(_ui->lineEdit_2->text() + "/" + project.GetName() + ".hod");

	// Hum parent ? Prefer a MainWindow Singleton
	if (static_cast<MainWindow*>(parentWidget())->LoadProjectAtPath(project.GetSavePath()) == false)
	{
		return false;
	}

	close();
	return true;
}
