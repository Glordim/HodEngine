#include "SceneContent.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "../ApplicationDescription.h"

/// @brief Construct a new SceneContent object
SceneContent::SceneContent()
{
}

/// @brief 
/// @param data 
/// @return true 
/// @return false 
bool SceneContent::Serialize(QJsonObject& data)
{
	data["Name"] = GetName();

	QJsonArray entitiesArray;
	for (Entity* entity : _entities)
	{
		QJsonObject entityElement;
		entityElement["Name"] = entity->GetName();

		QJsonArray componentArray;

		for (Component* component : entity->_components)
		{
			QJsonObject componentElement;
			componentElement["TypeName"] = component->_typeName;

			QJsonArray propertiesArray;
			for (Property* property : component->_properties)
			{
				QJsonObject propertyElement;

				propertyElement["Name"] = property->_name;
				propertyElement["Value"] = property->_value;

				propertiesArray.append(propertyElement);
			}

			componentElement["Properties"] = propertiesArray;

			componentArray.append(componentElement);
		}

		entityElement["Components"] = componentArray;

		entitiesArray.append(entityElement);
	}

	data["Entities"] = entitiesArray;

	return true;
}

/// @brief 
/// @param data 
/// @return true 
/// @return false 
bool SceneContent::Deserialize(const QJsonObject& data)
{
	QJsonArray entitiesArray = data["Entities"].toArray();
	for (const QJsonValueRef& entityItem : entitiesArray)
	{
		QJsonObject entityObject = entityItem.toObject();

		Entity* entity = new Entity(this);
		entity->SetName(entityObject["Name"].toString());
		QJsonArray componentsArray = entityObject["Components"].toArray();
		for (const QJsonValueRef& componentItem : componentsArray)
		{
			QJsonObject componentObject = componentItem.toObject();

			Component* component = new Component(entity);
			component->_typeName = componentObject["TypeName"].toString();

			QJsonObject componentDescription = ApplicationDescription::GetInstance()->FindComponent(component->_typeName);
			if (componentDescription.isEmpty() == true)
			{
				continue; // skip obsolete component
			}

			QJsonArray propertiesArray = componentObject["Properties"].toArray();
			for (const QJsonValueRef& propertyItem : propertiesArray)
			{
				QJsonObject propertyObject = propertyItem.toObject();

				QString propertyName = propertyObject["Name"].toString();

				QJsonObject propertyDescription = ApplicationDescription::GetInstance()->FindComponentProperty(component->_typeName, propertyName);
				if (propertyDescription.isEmpty() == true)
				{
					continue; // skip obsolete property
				}

				Property* property = new Property(component);
				//property->_typeName = propertyObject["TypeName"];
				property->_name = propertyName;
				property->_value = propertyObject["Value"].toString();

				component->_properties.push_back(property);
			}

			QJsonArray propertiesDescriptionArray = componentDescription["Properties"].toArray();
			for (const QJsonValueRef& propertyDescriptionItem : propertiesDescriptionArray)
			{
				QJsonObject propertyDescriptionObject = propertyDescriptionItem.toObject();
				QString propertyDescriptionName = propertyDescriptionObject["Name"].toString();

				bool founded = false;
				for (Property* property : component->_properties)
				{
					if (property->_name == propertyDescriptionName)
					{
						founded = true;
						break;
					}
				}

				if (founded == false)
				{
					Property* property = new Property(component);
					//property->_typeName = propertyObject["TypeName"];
					property->_name = propertyDescriptionName;
					property->_value = "";

					component->_properties.push_back(property);
				}
			}

			entity->_components.push_back(component);
		}

		_entities.push_back(entity);
	}

	return true;
}

/// @brief 
/// @return 
const QVector<SceneContent::Entity*> SceneContent::GetEntities() const
{
	return _entities;
}

/// @brief 
/// @return 
SceneContent::Entity* SceneContent::CreateEntity()
{
	SetDirty();

	Entity* entity = new Entity(this);
	_entities.push_back(entity);

	return entity;
}

SceneContent::Entity::Entity(SceneContent* scene)
	: _scene(scene)
{

}

SceneContent::Component* SceneContent::Entity::AddComponent(const QString& typeName)
{
	_scene->SetDirty();

	Component* component = new Component(this);
	component->_typeName = typeName;
	component->_description = ApplicationDescription::GetInstance()->FindComponent(typeName);
	_components.push_back(component);

	return component;
}

/// @brief 
/// @return 
const QString& SceneContent::Entity::GetName() const
{
	return _name;
}

/// @brief 
/// @param name 
void SceneContent::Entity::SetName(const QString& name)
{
	if (_name != name)
	{
		_name = name;
		_scene->SetDirty();
	}
}

SceneContent::Component::Component(Entity* entity)
	: _entity(entity)
{

}

SceneContent::Property::Property(Component* component)
	: _component(component)
{

}
