
#include "PropertyFloat32.h"

/// @brief 
/// @param property 
/// @param description 
PropertyFloat32::PropertyFloat32(SceneContent::Property* property, const QJsonObject& description)
	: Property(property, description)
{
	_spinbox.setValue(property->_value.toFloat());
	QObject::connect(&_spinbox, &QDoubleSpinBox::valueChanged, this, &PropertyFloat32::OnValueChanged);

	_mainLayout.addRow(description["Name"].toString(), &_spinbox);

	setLayout(&_mainLayout);
}

/// @brief 
/// @param value 
void PropertyFloat32::OnValueChanged(double value)
{
	_property->_value = QString::number(_spinbox.value());

	_property->_component->_entity->_scene->SetDirty();
}
