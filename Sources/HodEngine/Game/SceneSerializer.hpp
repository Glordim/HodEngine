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
		bool SerializeEntity(const std::shared_ptr<Entity> entity, bool withChildren, Document::Node& entityNode, uint64_t& nextLocalId);

		bool	Deserialize(const Document::Node& entitiesNode);

		const std::unordered_map<uint64_t, std::shared_ptr<Entity>>& GetEntities() const;
		const std::unordered_map<uint64_t, std::shared_ptr<Component>>& GetComponents() const;

	private:

		bool	DeserializeEntity(const Document::Node& entityNode);
		bool	DeserializeEntityRaw(const Document::Node& entityNode);
		bool	DeserializeEntityPrefab(const Document::Node& entityNode);

	private:

		std::unordered_map<uint64_t, std::shared_ptr<Entity>>		_entities;
		std::unordered_map<uint64_t, std::shared_ptr<Component>>	_components;
	};
}
