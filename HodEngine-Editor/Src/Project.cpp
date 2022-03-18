
#include "Project.h"
#include "ContentDatabase.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

#include <filesystem>

template<>
Project* Singleton<Project>::_instance = nullptr;

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool Project::IsOpened() const
{
	return _isOpened;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Project::Close()
{
	_isOpened = false;

	ContentDataBase::GetInstance()->Unload();

	_unloadProjectSignal.Emit(this);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool Project::LoadFromFile(const QString& projectFilePath)
{
	_filePath = projectFilePath;

	QFile file(_filePath);
	if (file.open(QIODevice::ReadOnly) == false)
	{
		qWarning("Couldn't open save file.");
		return false;
	}

	QByteArray saveData = file.readAll();
	file.close();

	QJsonDocument jsonDocument = QJsonDocument::fromJson(saveData);
	QJsonObject root = jsonDocument.object();
	_name = root["Name"].toString();

	_isOpened = true;

	ContentDataBase::GetInstance()->Load(GetContentsFolderPath());

	_loadProjectSignal.Emit(this);

	return true;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool Project::SaveAtPath(const QString& projectFilePath)
{
	_filePath = projectFilePath;
	_isDirty = false;

	QJsonObject root;
	root["Name"] = _name;

	QFile file(_filePath);
	if (file.open(QIODevice::WriteOnly) == false)
	{
		qWarning("Couldn't open save file.");
		return false;
	}

	file.write(QJsonDocument(root).toJson());
	file.close();

	return true;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool Project::CreateOnDisk(const QString& name, const QString& location)
{
	QDir dir(location);
	if (dir.exists() == false)
	{
		QMessageBox::critical(nullptr, "Project", "Unable to create project\n'" + dir.path() + "' directory does'nt exist");
		return false;
	}

	Project project;
	project.SetName(name);
	if (project.SaveAtPath(dir.path() + "/" + name + ".hod") == false)
	{
		QMessageBox::critical(nullptr, "Project", "Unable to create project file\nPath : '" + project.GetSavePath() + "'");
		return false;
	}

	if (dir.mkdir("Assets") == false)
	{
		QMessageBox::critical(nullptr, "Project", "Unable to create 'Assets' directory\nPath : '" + dir.path() + "'");
		return false;
	}

	if (dir.mkdir("Contents") == false)
	{
		QMessageBox::critical(nullptr, "Project", "Unable to create 'Contents' directory\nPath : '" + dir.path() + "'");
		return false;
	}

	if (dir.mkdir("Resources") == false)
	{
		QMessageBox::critical(nullptr, "Project", "Unable to create 'Resources' directory\nPath : '" + dir.path() + "'");
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Project::RegisterLoadProject(typename LoadProjectSignal::Slot& slot)
{
	_loadProjectSignal.Connect(slot);
}
//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Project::UnregisterLoadProject(typename LoadProjectSignal::Slot& slot)
{
	_loadProjectSignal.Disconnect(slot);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Project::RegisterUnloadProject(typename UnloadProjectSignal::Slot& slot)
{
	_unloadProjectSignal.Connect(slot);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Project::UnregisterUnloadProject(typename UnloadProjectSignal::Slot& slot)
{
	_unloadProjectSignal.Disconnect(slot);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
QString Project::GetAssetsFolderPath() const
{
	return _filePath.left(_filePath.lastIndexOf('/')) + "/Assets";
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
QString Project::GetContentsFolderPath() const
{
	return _filePath.left(_filePath.lastIndexOf('/')) + "/Contents";
}
