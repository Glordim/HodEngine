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

		void				SetName(const std::string_view& name);
		const std::string&	GetName() const;

		bool			SerializeInDocument(Document::Node& documentNode);
		bool			DeserializeFromDocument(const Document::Node& documentNode);

		Entity*							CreateEntity(const std::string_view& name = "");
		void							DestroyEntity(Entity* entity);
		Entity*							FindEntity(uint64_t entityId);

		void							Clear();

		Entity*							GetRootEntity();

		const std::unordered_map<uint64_t, Entity*>& GetEntities() const;

		void						SetNextLocalId(uint64_t nextLocalId) { _nextLocalId = nextLocalId; }
		uint64_t					GetNextLocalId() const { return _nextLocalId; }

	private:

		std::string												_name;
		std::unordered_map<uint64_t, Entity*>					_entities;
		Entity*													_rootEntity;
		uint64_t												_nextLocalId = 1;
	};
}
