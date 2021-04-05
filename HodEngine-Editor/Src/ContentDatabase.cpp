#include <QDir>
#include <QFile>
#include <QMessageBox>

#include "ContentDatabase.h"
#include "Project.h"
#include "UID.h"

ContentDataBase* Singleton<ContentDataBase>::_instance = nullptr;

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

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void ContentDataBase::AddContent(Content* content)
{
	if (_contents.contains(content->GetUID()) == false)
	{
		_contents.insert(content->GetUID(), content);

		_addContentSignal.Emit(content);
	}
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
Content* ContentDataBase::GetContent(const UID& uid) const
{
	QMap<UID, Content*>::ConstIterator it = _contents.find(uid);

	if (it != _contents.end())
	{
		return _contents.find(uid).value();
	}

	return nullptr;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void ContentDataBase::OnLoadProjectAction(Project* project)
{

}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void ContentDataBase::OnUnLoadProjectAction(Project* project)
{
}
