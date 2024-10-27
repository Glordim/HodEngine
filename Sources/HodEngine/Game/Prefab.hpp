#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Document/Document.hpp>

#include "HodEngine/Game/Entity.hpp"

#include <map>
#include <unordered_map>
#include <memory>

namespace hod::game
{
	/// @brief 
	class HOD_GAME_API Prefab
	{
		REFLECTED_CLASS_NO_PARENT(Prefab, HOD_GAME_API)

	public:
						Prefab() = default;
						Prefab(const UID& uid);
						Prefab(const Prefab&) = delete;
						Prefab(Prefab&&) = delete;
		virtual			~Prefab() = default;

		Prefab&			operator=(const Prefab&) = delete;
		Prefab&			operator=(Prefab&&) = delete;

	public:

		void				SetName(const std::string_view& name);
		const std::string&	GetName() const;

		bool			SerializeInDocument(Document::Node& documentNode) const;
		bool			DeserializeFromDocument(const Document::Node& documentNode);

		std::weak_ptr<Entity>			CreateEntity(const std::string_view& name = "");
		void							DestroyEntity(std::shared_ptr<Entity> entity);
		std::weak_ptr<Entity>			FindEntity(Entity::Id entityId);

		void							Clear();

		std::shared_ptr<Entity>			GetRootEntity();

		const std::unordered_map<Entity::Id, std::shared_ptr<Entity>>& GetEntities() const;

	private:

		std::string												_name;
		std::unordered_map<Entity::Id, std::shared_ptr<Entity>>	_entities;
	};
}
