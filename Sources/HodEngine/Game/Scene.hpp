#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

#include "HodEngine/Game/Entity.hpp"

#include <map>
#include <memory>
#include <unordered_map>

namespace hod::inline game
{
	class PrefabResource;
	class Entity;
	class Component;
	class World;

	/// @brief
	class HOD_GAME_API Scene
	{
		REFLECTED_CLASS_NO_PARENT(Scene)

	public:
		Scene();
		Scene(const Scene&) = delete;
		Scene(Scene&&) = delete;
		virtual ~Scene();

		Scene& operator=(const Scene&) = delete;
		Scene& operator=(Scene&&) = delete;

	public:
		void          SetName(const std::string_view& name);
		const String& GetName() const;

		bool SerializeInDocument(DocumentNode& documentNode);
		bool DeserializeFromDocument(const DocumentNode& documentNode);

		Entity* CreateEntity(const std::string_view& name = "");
		void    DestroyEntity(Entity* entity);
		Entity* FindEntity(uint64_t entityId);

		Scene* Clone(World* newWorld);
		void   Clear();
		/// @brief Destructs and deletes all its entities (as its destruction does), the scene left empty
		void   DestroyEntities();

		Entity* Instantiate(std::shared_ptr<PrefabResource> prefabResource);
		Entity* Instantiate(Entity* entity);
		/// @brief The entities of a document written by SceneSerializer::SerializeEntity (its children), added to the scene and activated.
		/// Their local ids are the document's: cleared by the caller if they may already exist in the scene
		/// @return the roots among them (without parent), in the document's order
		Vector<Entity*> Instantiate(const DocumentNode& entitiesNode);

		const std::unordered_map<uint64_t, Entity*>& GetEntities() const;
		/// @brief The entities without parent, in their order: the document's for the loaded ones, then by arrival (created, parent
		/// removed...), changed by Entity::SetSiblingIndex. What it serializes, in that order (see SerializeInDocument)
		Vector<Entity*> GetRootEntities() const;

		void ProcessActivation();
		void Update(float deltaTime);
		void FixedUpdate();

		void SetNextLocalId(uint64_t nextLocalId)
		{
			_nextLocalId = nextLocalId;
		}

		uint64_t GetNextLocalId() const
		{
			return _nextLocalId;
		}

		void   SetWorld(World* world);
		World* GetWorld() const;

	private:
		friend class Entity; // Keeps _rootEntities up to date (SetParent, SetScene, SetSiblingIndex)

		void     AddRoot(Entity* entity);
		void     RemoveRoot(Entity* entity);
		uint32_t GetRootIndex(const Entity* entity) const;
		void     SetRootIndex(Entity* entity, uint32_t index);
		/// @brief Puts roots, in that order, at the end of the roots
		void     MoveRootsToEnd(const Vector<Entity*>& roots);

	private:
		String                                _name;
		std::unordered_map<uint64_t, Entity*> _entities;
		Vector<Entity*>                       _rootEntities;
		uint64_t                              _nextLocalId = 1;
		World*                                _world = nullptr;
	};
}
