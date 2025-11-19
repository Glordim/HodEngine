#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Document/Document.hpp>

#include "HodEngine/Core/Vector.hpp"
#include <memory>

namespace hod::game
{
	class Entity;
	class Component;
	class WeakComponentBase;

	class HOD_GAME_API SceneSerializer
	{
	public:
		bool	SerializeEntity(Entity* entity, bool withChildren, Document::Node& entityNode, uint64_t& nextLocalId);

		bool	Deserialize(const Document::Node& entitiesNode);

		const Vector<Entity*>&		GetEntities() const;
		const Vector<Component*>&	GetComponents() const;

	private:

		struct HOD_GAME_API EntityComponentRoute final
		{
			uint64_t _entityId;
			uint64_t _componentId;
		};

	private:

		bool	SerializeEntityRaw(Entity* entity, Document::Node& entityNode, uint64_t& nextLocalId);
		bool	SerializeEntityPrefab(Entity* entity, Document::Node& entityNode, uint64_t& nextLocalId);

		bool	DeserializeEntity(const Document::Node& entityNode);
		bool	DeserializeEntityRaw(const Document::Node& entityNode);
		bool	DeserializeEntityPrefab(const Document::Node& entityNode);

		bool	CustomComponentSerializationCallback(const void* instance, const ReflectionDescriptor& reflectionDescriptor, Document::Node& documentNode);
		bool	CustomComponentDeserializationCallback(void* instance, const ReflectionDescriptor& reflectionDescriptor, const Document::Node& documentNode);

	private:

		Vector<Entity*>	_totalEntities;
		Vector<Component*>	_totalComponent;
		Vector<std::pair<WeakComponentBase*, Vector<EntityComponentRoute>>> _weakComponentsToResolve;

		std::unordered_map<uint64_t, Entity*>		_contextualEntityMap;
		std::unordered_map<uint64_t, Component*>	_contextualComponentMap;
	};
}
