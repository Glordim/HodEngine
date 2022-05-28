
#include "ApplicationDescription.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

#include <filesystem>

template<>
ApplicationDescription* Singleton<ApplicationDescription>::_instance = nullptr;

/// @brief 
/// @param descriptionFilePath 
/// @return 
bool ApplicationDescription::LoadFromFile(const QString& descriptionFilePath)
{
	QFile file(descriptionFilePath);
	if (file.open(QIODevice::ReadOnly) == false)
	{
		qWarning("Couldn't open description file.");
		return false;
	}

	QByteArray saveData = file.readAll();
	file.close();

	_document = QJsonDocument::fromJson(saveData);

	return true;
}

/// @brief 
/// @return 
QJsonArray ApplicationDescription::GetComponentsArray() const
{
	QJsonObject::const_iterator it = _document.object().constFind("Components");
	if (it == _document.object().constEnd())
	{
		qDebug("No Components array in ApplicationDescription");
	}

	return it->toArray();
}

/// @brief 
/// @param typeName 
/// @return 
QJsonObject ApplicationDescription::FindComponent(const QString& typeName)
{
	QJsonArray components = GetComponentsArray();
	QJsonArray::const_iterator it = components.begin();
	QJsonArray::const_iterator itEnd = components.end();

	while (it != itEnd)
	{
		QJsonObject component = it->toObject();
		if (component["TypeName"].toString() == typeName)
		{
			return component;
		}

		++it;
	}

	qDebug() << "No Component " << typeName << " founded in ApplicationDescription";
	return QJsonObject();
}

/// @brief 
/// @param componentTypeName 
/// @param propertyName 
/// @return 
QJsonObject ApplicationDescription::FindComponentProperty(const QString& componentTypeName, const QString& propertyName)
{
	QJsonObject component = FindComponent(componentTypeName);

	if (component.isEmpty() == true)
	{
		return QJsonObject();
	}

	QJsonArray properties = component["Properties"].toArray();
	QJsonArray::const_iterator it = properties.begin();
	QJsonArray::const_iterator itEnd = properties.end();

	while (it != itEnd)
	{
		QJsonObject property = it->toObject();
		if (property["Name"].toString() == propertyName)
		{
			return property;
		}

		++it;
	}

	qDebug() << "No Property " << componentTypeName << "::" << propertyName << " founded in ApplicationDescription";
	return QJsonObject();
}
