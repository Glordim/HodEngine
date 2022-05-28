#pragma once

#include <QDoubleSpinBox>
#include <QHBoxLayout>

#include "Property.h"

class PropertyVector2f32 : public Property
{
	Q_OBJECT

public:

	explicit PropertyVector2f32(SceneContent::Property* property, const QJsonObject& description);

private slots:

	void			OnValueChanged(double value);

protected:

	QHBoxLayout		_hLayout;
	QDoubleSpinBox	_spinboxX;
	QDoubleSpinBox	_spinboxY;
};
