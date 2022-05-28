
#include "Property.h"

Property::Property(SceneContent::Property* property, const QJsonObject& description)
	: QWidget()
	, _property(property)
	, _description(description)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
