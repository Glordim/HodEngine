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
{
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
ContentDataBase::~ContentDataBase()
{
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
		//qDebug() << "F: " + fileInfo.filePath();

		Content* content = ContentFactory::LoadFromPath(fileInfo.filePath());
		if (content != nullptr)
		{
			AddContent(content);
		}
		// read content here, construct and add it in Content map
	}

	dir.setFilter(QDir::Dirs | QDir::Filter::NoDotAndDotDot);

	for (QFileInfo dirInfo : dir.entryInfoList())
	{
		//qDebug() << "D: " + dirInfo.filePath();
		//qDebug() << "D: " + dirInfo.fileName();

		Load(dirInfo.filePath());
	}

	return true;
}

///
///@brief 
///
///
void ContentDataBase::Unload()
{
	_contents.clear();
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void ContentDataBase::AddContent(Content* content)
{
	auto existingContent = _contents.find(content->GetUID());

	if (existingContent != _contents.end())
	{
		QString contentsRootPath = Project::GetInstance()->GetContentsFolderPath();
		QString newContentPath = content->GetPath().mid(contentsRootPath.size() + 1);
		QString existingContentPath = (*existingContent)->GetPath().mid(contentsRootPath.size() + 1);

		int choice = QMessageBox::question(nullptr, "ContentDatabase - Duplicate UID",
		"A duplication of UIDs has been detected !\n\n'A' = " + newContentPath + "\n'B' = " + existingContentPath + "\n\nPlease indicate the content whose UID you want to preserve.\nThe other content will be assigned a new UID.",
		"Keep A",
		"Keep B");

		if (choice == 0)
		{
			content->SetUid(UID::GenerateUID());
			_contents.insert(content->GetUID(), content);
		}
		else
		{
			(*existingContent)->SetUid(UID::GenerateUID());
			_contents.insert((*existingContent)->GetUID(), (*existingContent));
			_contents[content->GetUID()] = content;
		}
	}
	else
	{
		_contents.insert(content->GetUID(), content);
	}

	_addContentSignal.Emit(content);
}

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

void ContentDataBase::DuplicateContent(const Content* content)
{
	/*
	QFile file(content.GetPath());
	QFileInfo fileInfo(content.GetPath());
	file.copy(fileInfo.completeBaseName() + " (copy).content");

	Content* content = ContentFactory::LoadFromPath(fileInfo.filePath());
	if (content != nullptr)
	{
		AddContent(content);
	}
	*/
}
