#include "HodEngine/Game/Scene.hpp"
#include "HodEngine/Game/Prefab.hpp"
#include "HodEngine/Game/Entity.hpp"

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/WeakComponent.hpp"
#include "HodEngine/Game/Components/RendererComponent.hpp"
#include "HodEngine/Game/Components/NodeComponent.hpp"

#include "HodEngine/Core/Reflection/ReflectionHelper.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Core/Output.hpp"
#include "HodEngine/Core/Color.hpp"

#include <unordered_map>

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS_NO_PARENT(Scene)
	{
		AddTrait<ReflectionTraitCustomSerialization>(
		[](const void* instance, Document::Node& documentNode)
		{
			static_cast<const Scene*>(instance)->SerializeInDocument(documentNode);
		},
		[](void* instance, const Document::Node& documentNode)
		{
			static_cast<Scene*>(instance)->DeserializeFromDocument(documentNode);
		});
	}

	/// @brief 
	/// @param name 
	void Scene::SetName(const std::string_view& name)
	{
		_name = name;
	}

	/// @brief 
	/// @return 
	const std::string& Scene::GetName() const
	{
		return _name;
	}

	/// @brief 
	/// @param documentNode 
	bool Scene::SerializeInDocument(Document::Node& documentNode) const
	{
		documentNode.AddChild("Name").SetString(_name);

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
				Serializer::Serialize(componentLock.get(), componentNode);
			}
		}

		return true;
	}

	/// @brief 
	/// @param documentNode 
	bool Scene::DeserializeFromDocument(const Document::Node& documentNode)
	{
		const Document::Node* nameNode = documentNode.GetChild("Name");
		if (nameNode!= nullptr)
		{
			_name = nameNode->GetString();
		}

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
					std::weak_ptr<Component> component = entityLock->AddComponent(componentDescriptor, false);
					std::shared_ptr<Component> componentLock = component.lock();
					Component* rawComponent = componentLock.get();
					Serializer::Deserialize(rawComponent, *componentNode); // todo lvalue...
					WeakComponentMapping::Insert(componentLock->GetUid(), component);
				}
				else
				{
					OUTPUT_ERROR("Unknown component !"); // TODO dangerous !!! user lost data
				}

				componentNode = componentNode->GetNextSibling();
			}

			entityNode = entityNode->GetNextSibling();
		}

		for (const auto& entity : _entities)
		{
			for (std::weak_ptr<Component> component : entity.second->GetComponents())
			{
				component.lock()->Construct();
			}
		}

		for (const auto& entity : _entities)
		{
			entity.second->SetActive(true);
		}

		return true;
	}

	/// @brief 
	/// @return 
	const std::unordered_map<Entity::Id, std::shared_ptr<Entity>>& Scene::GetEntities() const
	{
		return _entities;
	}

	/// @brief 
	/// @param name 
	/// @return 
	std::weak_ptr<Entity> Scene::CreateEntity(const std::string_view& name)
	{
		std::shared_ptr<Entity> entity = std::make_shared<Entity>(name);
		_entities.emplace(entity->GetId(), entity);

		return entity;
	}

	/// @brief 
	/// @param entity 
	void Scene::DestroyEntity(std::shared_ptr<Entity> entity)
	{
		_entities.erase(_entities.find(entity->GetId()));
	}

	/// @brief 
	/// @param entityId 
	/// @return 
	std::weak_ptr<Entity> Scene::FindEntity(Entity::Id entityId)
	{
		return _entities[entityId];
	}

	/// @brief 
	void Scene::Clear()
	{
		_entities.clear();
	}

	/// @brief 
	void Scene::Awake()
	{
		for (const auto& entity : _entities)
		{
			entity.second->Awake();
		}
	}

	///
	void Scene::Start()
	{
		for (const auto& entity : _entities)
		{
			entity.second->Start();
		}
	}

	/// @brief 
	void Scene::Update()
	{
		for (auto entityPair : _entities)
		{
			for (std::weak_ptr<Component> component : entityPair.second->GetComponents())
			{
				component.lock()->OnUpdate();
			}
		}
	}

	/// @brief 
	/// @param renderQueue 
	void Scene::Draw(renderer::RenderQueue* renderQueue)
	{
		for (const auto& pair : _entities)
		{
			std::shared_ptr<Entity> entity = pair.second;
			std::shared_ptr<RendererComponent> rendererComponent = entity->GetComponent<RendererComponent>().lock();
			if (rendererComponent != nullptr)
			{
				rendererComponent->PushToRenderQueue(*renderQueue);
			}
		}
	}

	/// @brief 
	/// @param renderQueue 
	/// @param colorIdToRendererComponentMap 
	void Scene::DrawPicking(renderer::RenderQueue* renderQueue, std::map<uint32_t, std::shared_ptr<RendererComponent>>& colorIdToRendererComponentMap, uint32_t& id)
	{
		union IdToColorConverter
		{
			uint32_t uint32;
			uint8_t uint8[4];
		};
		IdToColorConverter idToColorConverter;

		for (const auto& pair : _entities)
		{
			std::shared_ptr<Entity> entity = pair.second;
			std::shared_ptr<RendererComponent> rendererComponent = entity->GetComponent<RendererComponent>().lock();
			if (rendererComponent != nullptr)
			{
				idToColorConverter.uint32 = id;

				Color colorId;
				colorId.r = static_cast<float>(idToColorConverter.uint8[0]) / 255.0f;
				colorId.g = static_cast<float>(idToColorConverter.uint8[1]) / 255.0f;
				colorId.b = static_cast<float>(idToColorConverter.uint8[2]) / 255.0f;
				colorId.a = 1.0f;

				rendererComponent->PushPickingToRenderQueue(*renderQueue, colorId);

				colorIdToRendererComponentMap.emplace(idToColorConverter.uint32, rendererComponent);

				id += 50;
			}
		}
	}

	/// @brief 
	/// @return 
	Scene* Scene::Clone()
	{
		Scene* clone = new Scene();

		std::map<std::shared_ptr<Entity>, std::shared_ptr<Entity>> sourceToCloneEntitiesMap;		
		std::map<std::shared_ptr<Component>, std::shared_ptr<Component>> sourceToCloneComponentsMap;

		clone->_entities.reserve(_entities.size());
		for (const auto& entityPair : _entities)
		{
			std::shared_ptr<Entity> cloneEntity = entityPair.second->Clone();
			sourceToCloneEntitiesMap.emplace(entityPair.second, cloneEntity);

			for (size_t componentIndex = 0; componentIndex < cloneEntity->GetComponents().size(); ++componentIndex)
			{
				sourceToCloneComponentsMap.emplace(entityPair.second->GetComponents()[componentIndex], cloneEntity->GetComponents()[componentIndex]);
			}

			clone->_entities.emplace(cloneEntity->GetId(), cloneEntity);
		}
			
		for (const auto& componentPair : sourceToCloneComponentsMap)
		{
			ReflectionDescriptor* reflectionDescriptor = componentPair.second->GetReflectionDescriptorV();
			for (ReflectionProperty* reflectionProperty : reflectionDescriptor->GetProperties())
			{
				switch (reflectionProperty->GetMetaType())
				{
					case ReflectionPropertyArray::GetMetaTypeStatic():
					{
						//ReflectionPropertyArray* reflectionPropertyArray = static_cast<ReflectionPropertyArray*>(reflectionProperty);
					}
					break;

					case ReflectionPropertyObject::GetMetaTypeStatic():
					{
						//ReflectionPropertyObject* reflectionPropertyObject = static_cast<ReflectionPropertyObject*>(reflectionProperty);

						//if (reflectionPropertyObject->GetReflectionDescriptor() == WeakComponent::)
					}
					break;
				}
			}
		}

		return clone;
	}

	/// @brief 
	/// @param prefab 
	/// @return 
	std::shared_ptr<Entity> Scene::Instantiate(Prefab* prefab)
	{
		if (prefab == nullptr)
		{
			return nullptr;
		}
		std::shared_ptr<Entity> instance = Instantiate(prefab->GetRootEntity());
	#if defined(HOD_EDITOR)
		if (instance != nullptr)
		{
			instance->SetPrefab(prefab);
		}
	#endif
		return instance;
	}

	/// @brief 
	/// @param entity 
	/// @param sourceToCloneEntitiesMap 
	/// @param sourceToCloneComponentsMap 
	/// @return 
	std::shared_ptr<Entity> Scene::InstantiateInternal(std::shared_ptr<Entity> entity, std::unordered_map<std::shared_ptr<Entity>, std::shared_ptr<Entity>>& sourceToCloneEntitiesMap, std::unordered_map<std::shared_ptr<Component>, std::shared_ptr<Component>>& sourceToCloneComponentsMap)
	{
		std::shared_ptr<Entity> clonedEntity = entity->Clone();

		_entities.emplace(clonedEntity->GetId(), clonedEntity);
		sourceToCloneEntitiesMap.emplace(entity, clonedEntity);

		for (size_t componentIndex = 0; componentIndex < entity->GetComponents().size(); ++componentIndex)
		{
			sourceToCloneComponentsMap.emplace(entity->GetComponents()[componentIndex], clonedEntity->GetComponents()[componentIndex]);
		}
		
		std::shared_ptr<NodeComponent> nodeComponent = entity->GetComponent<NodeComponent>().lock();
		uint32_t childCount = nodeComponent->GetChildCount();
		for (uint32_t childIndex = 0; childIndex < childCount; ++childCount)
		{
			const std::shared_ptr<Entity> childEntity = nodeComponent->GetChild(childIndex).Lock()->GetEntity().lock();
			InstantiateInternal(childEntity, sourceToCloneEntitiesMap, sourceToCloneComponentsMap);
		}
		return clonedEntity;
	}

	/// @brief 
	/// @param entity 
	/// @return 
	std::shared_ptr<Entity> Scene::Instantiate(std::shared_ptr<Entity> entity)
	{
		if (entity == nullptr)
		{
			return nullptr;
		}

		std::unordered_map<std::shared_ptr<Entity>, std::shared_ptr<Entity>> sourceToCloneEntitiesMap;		
		std::unordered_map<std::shared_ptr<Component>, std::shared_ptr<Component>> sourceToCloneComponentsMap;

		std::shared_ptr<Entity> clonedEntity = InstantiateInternal(entity, sourceToCloneEntitiesMap, sourceToCloneComponentsMap);

		for (const auto& componentPair : sourceToCloneComponentsMap)
		{
			ReflectionDescriptor* reflectionDescriptor = componentPair.second->GetReflectionDescriptorV();
			for (ReflectionProperty* reflectionProperty : reflectionDescriptor->GetProperties())
			{
				switch (reflectionProperty->GetMetaType())
				{
					case ReflectionPropertyArray::GetMetaTypeStatic():
					{
						//ReflectionPropertyArray* reflectionPropertyArray = static_cast<ReflectionPropertyArray*>(reflectionProperty);
					}
					break;

					case ReflectionPropertyObject::GetMetaTypeStatic():
					{
						//ReflectionPropertyObject* reflectionPropertyObject = static_cast<ReflectionPropertyObject*>(reflectionProperty);

						//if (reflectionPropertyObject->GetReflectionDescriptor() == WeakComponent::)
					}
					break;
				}
			}
		}

		return clonedEntity;
	}
}
