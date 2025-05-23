#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Document/Document.hpp>

#include <vector>
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

		const std::vector<Entity*>&		GetEntities() const;
		const std::vector<Component*>&	GetComponents() const;

	private:

		bool	DeserializeEntity(const Document::Node& entityNode);
		bool	DeserializeEntityRaw(const Document::Node& entityNode);
		bool	DeserializeEntityPrefab(const Document::Node& entityNode);

	private:

		std::vector<Entity*>	_totalEntities;
		std::vector<Component*>	_totalComponent;

		std::unordered_map<uint64_t, Entity*>		_contextualEntityMap;
		std::unordered_map<uint64_t, Component*>	_contextualComponentMap;
	};
}
