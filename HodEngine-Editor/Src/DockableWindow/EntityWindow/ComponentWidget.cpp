
#include "ComponentWidget.h"

#include "../../ApplicationDescription.h"

#include "../../Widgets/PropertyFloat32.h"
#include "../../Widgets/PropertyVector2f32.h"

/// @brief 
/// @param component 
/// @param parent 
ComponentWidget::ComponentWidget(SceneContent::Component* component)
: Collaspsable(component->_typeName)
, _component(component)
{
	_layout = new QVBoxLayout();
	//_layout->setSizeConstraint(QLayout::SizeConstraint::QSizePolicy::Expanding, QSizePolicy::Expanding);

	for (SceneContent::Property* property : component->_properties)
	{
		QJsonObject propertyDescription = ApplicationDescription::GetInstance()->FindComponentProperty(component->_typeName, property->_name);

		QString type = propertyDescription["Type"].toString();

		if (type == "Vector2f32")
		{
			_layout->addWidget(new PropertyVector2f32(property, propertyDescription));
		}
		else if (type == "Float32")
		{
			_layout->addWidget(new PropertyFloat32(property, propertyDescription));
		}
	}

	/*
	QJsonObject componentDesscription = ApplicationDescription::GetInstance()->FindComponent(component->_typeName);

	QJsonArray propertiesArray = componentDesscription["Properties"].toArray();

	for (const QJsonValueRef& propertyElement : propertiesArray)
	{
		QJsonObject propertyDescription = propertyElement.toObject();
		QString typeName = propertyDescription["TypeName"].toString();

		if (typeName == "Variable")
		{
			QString name = propertyDescription["Name"].toString();
			QString type = propertyDescription["Type"].toString();

			SceneContent::Property* property = nullptr;
			for (SceneContent::Property* propertyIt : component->_properties)
			{
				if (propertyIt->_name == name && propertyIt->_typeName == type)
				{
					property = propertyIt;
					break;
				}
			}
			
			if (type == "Vector2f32")
			{
				_layout->addWidget(new PropertyVector2f32(property, propertyDescription));
			}
			else if (type == "Float32")
			{
				_layout->addWidget(new PropertyFloat32(property, propertyDescription));
			}
		}
	}
	*/
	setContentLayout(*_layout);
}

/// @brief 
ComponentWidget::~ComponentWidget()
{
}

