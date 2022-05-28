
#include "PropertyVector2f32.h"

#include <QJsonArray>
#include <QJsonDocument>

/// @brief 
/// @param property 
/// @param description 
PropertyVector2f32::PropertyVector2f32(SceneContent::Property* property, const QJsonObject& description)
	: Property(property, description)
{
	QJsonDocument document = QJsonDocument::fromJson(property->_value.toUtf8());
	QJsonArray array = document.array();

	if (array.count() == 2)
	{
		_spinboxX.setValue(array[0].toDouble());
		_spinboxY.setValue(array[1].toDouble());
	}

	QObject::connect(&_spinboxX, &QDoubleSpinBox::valueChanged, this, &PropertyVector2f32::OnValueChanged);
	QObject::connect(&_spinboxY, &QDoubleSpinBox::valueChanged, this, &PropertyVector2f32::OnValueChanged);

	_hLayout.addWidget(&_spinboxX);
	_hLayout.addWidget(&_spinboxY);

	_mainLayout.addRow(description["Name"].toString(), &_hLayout);

	setLayout(&_mainLayout);
}

/// @brief 
/// @param value 
void PropertyVector2f32::OnValueChanged(double value)
{
	QJsonArray array;

	array.append(_spinboxX.value());
	array.append(_spinboxY.value());

	QJsonDocument document;
	document.setArray(array);
	_property->_value = document.toJson(QJsonDocument::Compact);

	_property->_component->_entity->_scene->SetDirty();
}
