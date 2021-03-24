
#include "Project.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
Project::Project()
{

}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
Project::~Project()
{
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

	QJsonDocument jsonDocument = QJsonDocument::fromJson(saveData);
	QJsonObject root = jsonDocument.object();
	_name = root["Name"].toString();

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

	return true;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
const QString& Project::GetSavePath() const
{
	return _filePath;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool Project::IsDirty() const
{
	return _isDirty;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
const QString& Project::GetName() const
{
	return _name;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Project::SetName(const QString& name)
{
	_name = name;
	_isDirty = true;
}
