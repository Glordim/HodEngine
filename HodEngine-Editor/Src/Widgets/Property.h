#pragma once

#include <QFormLayout>
#include <QWidget>
#include <QJsonObject>

#include "../Contents/SceneContent.h"

class Property : public QWidget
{
	Q_OBJECT

public:

	explicit Property(SceneContent::Property* property, const QJsonObject& description);

protected:

	QFormLayout	_mainLayout;

	SceneContent::Property* _property;
	QJsonObject				_description;
};
