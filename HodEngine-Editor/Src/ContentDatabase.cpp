#include <QDir>
#include <QFile>
#include <QMessageBox>

#include "Contents/ContentFactory.h"
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
	Project::GetInstance()->RegisterLoadProject(_onLoadProjectSlot);
	Project::GetInstance()->RegisterUnLoadProject(_onUnLoadProjectSlot);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
ContentDataBase::~ContentDataBase()
{
	Project::GetInstance()->UnRegisterLoadProject(_onLoadProjectSlot);
	Project::GetInstance()->UnRegisterUnLoadProject(_onUnLoadProjectSlot);
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
		qDebug() << "F: " + fileInfo.filePath();

		Content* content = ContentFactory::LoadFromPath(fileInfo.filePath());
		if (content != nullptr)
		{
			AddContent(content);
		}
		// read content here, construct and add it in Content map
	}

	dir.setFilter(QDir::Dirs);

	for (QFileInfo dirInfo : dir.entryInfoList())
	{
		qDebug() << "D: " + dirInfo.filePath();
		qDebug() << "D: " + dirInfo.fileName();

		if (dirInfo.fileName() == "." || dirInfo.fileName() == "..")
		{
			continue;
		}

		Load(dirInfo.filePath());
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
void ContentDataBase::RemoveContent(Content* content)
{
	if (_contents.contains(content->GetUID()) == false)
	{
		qWarning("ContentDataBases::RemoveContent::Content is null");
		return;
	}
	
	_contents.remove(content->GetUID());
	_removeContentSignal.Emit(content);

	content->RemoveFile();
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

Content* ContentDataBase::GetContent(const QString path) const
{
	for (Content* content : _contents.values())
	{
		if (content != nullptr && content->GetPath() == path)
		{
			return content;
		}
	}

	return nullptr;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void ContentDataBase::OnLoadProjectAction(Project* project)
{
	Load(project->GetContentsFolderPath());
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void ContentDataBase::OnUnLoadProjectAction(Project* project)
{

}
