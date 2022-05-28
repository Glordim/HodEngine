#pragma once

#include <QDoubleSpinBox>

#include "Property.h"

class PropertyFloat32 : public Property
{
	Q_OBJECT

public:

	explicit		PropertyFloat32(SceneContent::Property* property, const QJsonObject& description);

private slots:

	void			OnValueChanged(double value);

protected:

	QDoubleSpinBox	_spinbox;
};
