#include "Content.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
Content::Content()
	: _isDirty(false)
	, _isValid(false)
	, _uid(UID::GenerateUID())
	, _name("")
{
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool Content::IsValid() const
{
	size_t vectorSize = _dependencies.size();

	for (uint32_t i = 0; i < vectorSize; ++i)
	{
		if (_dependencies[i]->IsValid() == false)
		{
			return false;
		}
	}

	return _assetPath.isEmpty() == false && _name.isEmpty() == false && _uid != UID::INVALID_UID;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Content::AddDependency(Content& dependency)
{
	_dependencies.push_back(&dependency);

	SetDirty();

	_addDependencySignal.Emit(&dependency);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Content::RemoveDependency(const UID& uid)
{
	size_t vectorSize = _dependencies.size();

	for (uint32_t i = 0; i < vectorSize; ++i)
	{
		if (_dependencies[i]->GetUID() == uid)
		{
			SetDirty();
			_removeDependencySignal.Emit(_dependencies[i]);
			_dependencies.erase(_dependencies.begin() + i);
			return;
		}
	}
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
const QVector<Content*>& Content::GetAllDependencies() const
{
	return _dependencies;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool Content::SaveAtPath(const QString& filepath)
{
	_path = filepath;
	_isDirty = false;

	QJsonObject root;

	QJsonObject header;
	SerializeHeader(header);
	root.insert("Header", header);

	QJsonObject data;
	Serialize(data);
	root.insert("Data", data);

	QFile file(_path);
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
bool Content::SerializeHeader(QJsonObject& header)
{
	header["Uid"] = _uid.ToString();
	header["Name"] = _name;
	header["Type"] = GetTypeName();
	header["Asset"] = GetAssetPath();
	header["Path"] = GetPath();

	QJsonArray dependenciesArray;

	for (Content* dependency : _dependencies)
	{
		dependenciesArray.append(dependency->GetUID().ToString());
	}

	header["Dependencies"] = dependenciesArray;

	return true;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool Content::DeserializeHeader(const QJsonObject& header)
{
	_uid = UID::FromString(header["Uid"].toString());
	_name = header["Name"].toString();
	_assetPath = header["Asset"].toString();
	_path = header["Path"].toString();

	QJsonArray dependencies = header["Dependencies"].toArray();

	for (qsizetype i = 0; i < dependencies.count(); ++i)
	{
		UID dependencyUID = UID::FromString(dependencies[i].toString());
		//Demande a la databse l'asset si il ne le trouve pas le charge sinon null
		// _dependencies
	}

	return true;
}
