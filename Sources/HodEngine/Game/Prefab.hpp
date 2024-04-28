#pragma once

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Document/Document.hpp>

#include "HodEngine/Game/Entity.hpp"

#include <map>
#include <unordered_map>
#include <memory>

namespace hod::game
{
	/// @brief 
	class Prefab
	{
		REFLECTED_CLASS_NO_PARENT(Prefab)

	public:
						Prefab() = default;
						Prefab(const UID& uid);
						Prefab(const Prefab&) = delete;
						Prefab(Prefab&&) = delete;
		virtual			~Prefab() = default;

		Prefab&			operator=(const Prefab&) = delete;
		Prefab&			operator=(Prefab&&) = delete;

	public:

		const UID&			GetUid() const;

		void				SetName(const std::string_view& name);
		const std::string&	GetName() const;

		bool			SerializeInDocument(Document::Node& documentNode) const;
		bool			DeserializeFromDocument(const Document::Node& documentNode);

		std::weak_ptr<Entity>			CreateEntity(const std::string_view& name = "");
		void							DestroyEntity(std::shared_ptr<Entity> entity);
		std::weak_ptr<Entity>			FindEntity(Entity::Id entityId);

		Prefab*							Clone();
		void							Clear();

		std::shared_ptr<Entity>			GetRootEntity();

		const std::unordered_map<Entity::Id, std::shared_ptr<Entity>>& GetEntities() const;

	private:

		UID														_uid;
		std::string												_name;
		std::unordered_map<Entity::Id, std::shared_ptr<Entity>>	_entities;
	};
}
