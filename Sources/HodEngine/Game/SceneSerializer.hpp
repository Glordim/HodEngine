#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Document/Document.hpp>

#include "HodEngine/Core/Vector.hpp"
#include <memory>

namespace hod::game
{
	class Entity;
	class Component;

	class HOD_GAME_API SceneSerializer
	{
	public:
		bool	SerializeEntity(Entity* entity, bool withChildren, Document::Node& entityNode, uint64_t& nextLocalId);

		bool	Deserialize(const Document::Node& entitiesNode);

		const Vector<Entity*>&		GetEntities() const;
		const Vector<Component*>&	GetComponents() const;

	private:

		bool	DeserializeEntity(const Document::Node& entityNode);
		bool	DeserializeEntityRaw(const Document::Node& entityNode);
		bool	DeserializeEntityPrefab(const Document::Node& entityNode);

	private:

		Vector<Entity*>	_totalEntities;
		Vector<Component*>	_totalComponent;

		std::unordered_map<uint64_t, Entity*>		_contextualEntityMap;
		std::unordered_map<uint64_t, Component*>	_contextualComponentMap;
	};
}
