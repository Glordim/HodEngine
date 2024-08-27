#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/SceneSerializer.hpp"

#include "HodEngine/Game/Entity.hpp"
#include "HodEngine/Game/Prefab.hpp"
#include "HodEngine/Game/PrefabUtility.hpp"

#include "HodEngine/Core/Serialization/Serializer.hpp"

namespace hod::game
{
    /// @brief 
    /// @param entities 
    /// @param entitiesNode 
    /// @return 
    bool SceneSerializer::SerializeEntities(const std::vector<std::shared_ptr<Entity>>& entities, Document::Node& entitiesNode)
    {
        for (std::shared_ptr<Entity> entity : entities)
		{
			Document::Node& entityNode = entitiesNode.AddChild("");

			Prefab* prefab = entity->GetPrefab();
			if (prefab != nullptr)
			{
				Document::Node& prefabInstance = entityNode.AddChild("PrefabInstance");
				UID uid = prefab->GetUid(); // TODO fix Serializa template lvalue
				Serializer::Serialize(uid, prefabInstance.AddChild("UID"));
				Document::Node& overridesNode = prefabInstance.AddChild("Overrides");

				PrefabUtility::EntityDiffs entityDiffs;
				PrefabUtility::CollectDiff(entity, entityDiffs);

				for (PrefabUtility::EntityDiffs::Diff* diff : entityDiffs._diffs)
				{
					Document::Node& overrideNode = overridesNode.AddChild("");
					Document::Node& overrideTargetNode = overrideNode.AddChild("Target");
					if (diff->_type == PrefabUtility::EntityDiffs::Diff::Type::Entity)
					{
						overrideTargetNode.AddChild("Type").SetInt64(static_cast<int64_t>(PrefabUtility::EntityDiffs::Diff::Type::Entity));
						uid = static_cast<Entity*>(diff->_instance)->GetLocalId();
						Serializer::Serialize(uid, overrideTargetNode.AddChild("UID"));
					}
					else // Component
					{
						overrideTargetNode.AddChild("Type").SetInt64(static_cast<int64_t>(PrefabUtility::EntityDiffs::Diff::Type::Component));
						uid = static_cast<Component*>(diff->_instance)->GetLocalId();
						Serializer::Serialize(uid, overrideTargetNode.AddChild("UID"));
					}
					// TODO can be an array of modifcations to mutalize target description
					Document::Node& overrideModificationNode = overrideNode.AddChild("Modification");
					Serializer::SerializeVariable((ReflectionPropertyVariable*)diff->_reflectionProperty, diff->_effectiveInstanceAddr, overrideModificationNode, diff->_path);
				}
			}
			else
			{
				Serializer::Serialize(entity.get(), entityNode);

				Document::Node& componentsNode = entityNode.AddChild("Components");

				const std::vector<std::weak_ptr<Component>> components =  entity->GetComponents();
				for (const std::weak_ptr<Component>& component : components)
				{
					std::shared_ptr<Component> componentLock = component.lock();

					Document::Node& componentNode = componentsNode.AddChild("");
					componentNode.AddChild("MetaType").SetUInt64(componentLock->GetMetaType());
					Serializer::Serialize(componentLock.get(), componentNode);
				}
			}
		}

        return true;
    }
}
