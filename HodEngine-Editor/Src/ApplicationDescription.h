#pragma once

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "Singleton.h"

/// @brief 
class ApplicationDescription : public Singleton<ApplicationDescription>
{
	friend class Singleton<ApplicationDescription>;

protected:

					ApplicationDescription() = default;
					ApplicationDescription(const ApplicationDescription&) = delete;
					ApplicationDescription(ApplicationDescription&&) = delete;
					~ApplicationDescription() = default;

	void			operator=(const ApplicationDescription&) = delete;
	void			operator=(ApplicationDescription&&) = delete;

public:

	bool			LoadFromFile(const QString& descriptionFilePath);

	QJsonArray		GetComponentsArray() const;
	QJsonObject		FindComponent(const QString& typeName);
	QJsonObject		FindComponentProperty(const QString& componentTypeName, const QString& propertyName);

private:

	QJsonDocument	_document;
};

#include "ApplicationDescription.inl"
