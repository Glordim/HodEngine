#include "HodEngine/Game/World.h"

#include "HodEngine/Game/Scene.h"
#include "HodEngine/Game/Builtin.h"

#include "HodEngine/Game/Component.h"
#include "HodEngine/Game/ComponentFactory.h"

#include "HodEngine/Core/Output.h"

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		_SingletonConstructor(World)
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
			/*
			for (Scene* scene : _scenes)
			{
				sScene->Update(dt);
			}
			*/
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
					componentLock->GetReflectionDescriptorV()->SerializeInDocument(reinterpret_cast<const void*>(componentLock.get()), componentNode);
				}
			}

			return true;
		}

		/// @brief 
		/// @param documentNode 
		/// @return 
		bool World::LoadFromDocument(const Document::Node& documentNode)
		{
			ComponentFactory* componentFactory = ComponentFactory::GetInstance();

			const Document::Node* entitiesNode = documentNode.GetChild("Entities");
			const Document::Node* entityNode = entitiesNode->GetFirstChild();
			while (entityNode != nullptr)
			{
				const std::string& name = entityNode->GetChild("Name")->GetString();

				std::weak_ptr<Entity> entity = CreateEntity(name);
				std::shared_ptr<Entity> entityLock = entity.lock();

				bool active = entityNode->GetChild("Active")->GetBool();
				entityLock->SetActive(active);

				const Document::Node* componentsNode = entityNode->GetChild("Components");
				const Document::Node* componentNode = componentsNode->GetFirstChild();
				while (componentNode != nullptr)
				{
					MetaType metaType = componentNode->GetChild("MetaType")->GetUInt64();

					auto it = componentFactory->GetAllDescriptors().find(metaType);
					if (it != componentFactory->GetAllDescriptors().end())
					{
						const ReflectionDescriptor& componentDescriptor = *it->second;
						std::weak_ptr<Component> component = entityLock->AddComponent(componentDescriptor);
						//componentDescriptor.DeserializeFromDocument(reinterpret_cast<void*>(component.lock().get()), componentNode);
					}
					else
					{
						OUTPUT_ERROR("Unknown component !"); // TODO dangerous !!! user lost data
					}					

					componentNode = componentNode->GetNextSibling();
				}

				entityNode = entityNode->GetNextSibling();
			}

			return true;
		}
	}
}
