#pragma once

#include <string>
#include <vector>

#include "../UID.h"
#include "../Signal.h"
#include "Content.h"

#include <QJsonObject>

/// @brief 
class SceneContent : public Content
{
	META_TYPE(SceneContent)

public:

	struct Property;
	struct Component;
	struct Entity;

	struct Property
	{
		Property(Component* component);

		//QString				_typeName;
		QString				_name;
		QString				_value;

		Component*			_component;
	};

	struct Component
	{
		Component(Entity* entity);

		QString				_typeName;
		QVector<Property*>	_properties;

		Entity*				_entity;
		QJsonObject			_description;
	};

	struct Entity
	{
		Entity(SceneContent* scene);

		Component*			AddComponent(const QString& typeName);
		const QString&		GetName() const;
		void				SetName(const QString& name);

		QVector<Component*>	_components;

		SceneContent*		_scene;

	private:
		QString				_name;
	};

public:

							SceneContent();
							SceneContent(const SceneContent&) = delete;
							SceneContent(SceneContent&&) = delete;
							~SceneContent() override = default;

	void					operator=(const SceneContent&) = delete;
	void					operator=(SceneContent&&) = delete;

public:

	bool					Serialize(QJsonObject& data) override;
	bool					Deserialize(const QJsonObject& data) override;

	const QVector<Entity*>	GetEntities() const;

	Entity*					CreateEntity();

private:

	QVector<Entity*>		_entities;
};
