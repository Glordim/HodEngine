#include "HodEngine/Game/World.h"

#include "HodEngine/Game/Scene.h"
#include "HodEngine/Game/Builtin.h"

#include "HodEngine/Game/Component.h"

namespace hod
{
	template<>
	game::World* Singleton<game::World>::_instance = nullptr;

	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		World::World() : Singleton()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		World::~World()
		{
			Clear();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool World::Init()
		{
			RegisterBuiltin();

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void World::Clear()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Scene* World::CreateScene()
		{
			Scene* pScene = new Scene();

			_scenes.push_back(pScene);

			return pScene;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void World::DestroyScene(Scene* pScene)
		{
			auto it = _scenes.begin();
			auto itEnd = _scenes.end();
			while (it != itEnd)
			{
				if (*it == pScene)
				{
					_scenes.erase(it);
					break;
				}
			}

			delete pScene;
		}

		/// @brief 
		/// @param scene 
		/// @return 
		bool World::AddScene(Scene* scene)
		{
			auto it = _scenes.begin();
			auto itEnd = _scenes.end();
			while (it != itEnd)
			{
				if (*it == scene)
				{
					// Todo message already
					return false;
				}
				++it;
			}

			_scenes.push_back(scene);
			return true;
		}

		/// @brief 
		/// @param scene 
		/// @return 
		bool World::RemoveScene(Scene* scene)
		{
			auto it = _scenes.begin();
			auto itEnd = _scenes.end();
			while (it != itEnd)
			{
				if (*it == scene)
				{
					_scenes.erase(it);
					return true;
				}
			}

			return false;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void World::Update(float dt)
		{
			for (Scene* pScene : _scenes)
			{
				//pScene->Update(dt);
			}
		}

		/// @brief 
		/// @return 
		const std::vector<Scene*>& World::GetScenes() const
		{
			return _scenes;
		}

		/// @brief 
		/// @param name 
		/// @return 
		std::weak_ptr<Entity> World::CreateEntity(const std::string_view& name)
		{
			std::shared_ptr<Entity> entity = std::make_shared<Entity>(name);
			_entities.emplace(entity->GetId(), entity);

			_addEntityEvent.Emit(entity);

			return entity;
		}

		/// @brief 
		/// @return 
		const std::unordered_map<Entity::Id, std::shared_ptr<Entity>>& World::GetEntities() const
		{
			return _entities;
		}

		/// @brief 
		/// @param entityId 
		/// @return 
		std::weak_ptr<Entity> World::FindEntity(Entity::Id entityId)
		{
			return _entities[entityId];
		}

		/// @brief 
		/// @param documentNode 
		/// @return 
		bool World::SaveToDocument(Document::Node& documentNode)
		{
			Document::Node& entitiesNode = documentNode.AddChild("Entities");

			for (const auto& entityPair : _entities)
			{
				std::shared_ptr<Entity> entity = entityPair.second;

				Document::Node& entityNode = entitiesNode.AddChild("");

				entityNode.AddChild("Name").SetString(entity->GetName());
				entityNode.AddChild("Active").SetBool(entity->GetActive());

				Document::Node& componentsNode = entityNode.AddChild("Components");

				const std::vector<std::weak_ptr<Component>> components =  entity->GetComponents();
				for (const std::weak_ptr<Component>& component : components)
				{
					std::shared_ptr<Component> componentLock = component.lock();

					Document::Node& componentNode = componentsNode.AddChild("");
					componentNode.AddChild("MetaType").SetValue(componentLock->GetMetaType());
					componentLock->GetReflectionDescriptorV()->SerializeInDocument(reinterpret_cast<void*>(&componentLock), componentNode);
				}
			}

			return true;
		}
	}
}
