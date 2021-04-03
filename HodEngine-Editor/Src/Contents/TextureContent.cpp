#include "TextureContent.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
TextureContent::TextureContent()
{
}

bool TextureContent::Serialize()
{
	QJsonObject root;
	QJsonArray dependencyArray;
	QFile file(_path);

	root["Name"] = _name;
	root["UID"] = _uid.ToString();
	root["AssetPath"] = _assetPath;


	if (file.open(QIODevice::WriteOnly) == false)
	{
		qWarning("Couldn't open save file.");
		return false;
	}

	for (Content* dependcy : _dependencies)
	{
		dependencyArray.append(dependcy->GetUID().ToString());
	}

	root["Dependencies"] = dependencyArray;

	file.write(QJsonDocument(root).toJson());
	file.close();

	return true;
}

bool TextureContent::Deserialize()
{
	QFile file(_path);

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
	_uid = UID::FromString(root["UID"].toString());
	_assetPath = root["AssetPath"].toString();
	
	QJsonArray dependencies = root["Dependencies"].toArray();

	for (qsizetype i = 0; i < dependencies.count(); ++i)
	{
		UID dependencyUID = UID::FromString(dependencies[i].toString());
		//Demande a la databse l'asset si il ne le trouve pas le charge sinon null
	}

	file.close();

	return true;
}
