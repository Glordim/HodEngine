#include <QDir>
#include <QFile>
#include <QMessageBox>

#include "ContentDatabase.h"
#include "Project.h"

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
ContentDataBase::ContentDataBase()
	: _onLoadProjectSlot(std::bind(&ContentDataBase::OnLoadProjectAction, this, std::placeholders::_1))
	, _onUnLoadProjectSlot(std::bind(&ContentDataBase::OnUnLoadProjectAction, this, std::placeholders::_1))
{
	Project::LoadProjectSignal().Connect(_onLoadProjectSlot);
	Project::UnLoadProjectSignal().Connect(_onUnLoadProjectSlot);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
ContentDataBase::~ContentDataBase()
{
	Project::LoadProjectSignal().Disconnect(_onLoadProjectSlot);
	Project::UnLoadProjectSignal().Disconnect(_onUnLoadProjectSlot);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool ContentDataBase::Load(const QString& contentFolderPath)
{
	QDir dir(contentFolderPath);
	if (dir.exists() == false)
	{
		QMessageBox::critical(nullptr, "ContentDataBase", "Unable to open content folder\n'" + dir.path() + "' directory does'nt exist");
		return false;
	}

	dir.setFilter(QDir::Files);

	for (QFileInfo fileInfo : dir.entryInfoList())
	{
		qDebug() << fileInfo.filePath();
	}

	return true;
}


Content* ContentDataBase::GetContent(UID& uid) const
{
	return nullptr;
}

void ContentDataBase::OnLoadProjectAction(Project* project)
{

}

void ContentDataBase::OnUnLoadProjectAction(Project* project)
{
}
