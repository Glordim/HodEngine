#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/PrefabInstance.hpp"
#include "HodEngine/UI2/Node.hpp"
#include "HodEngine/UI2/UIPrefabResource.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>

namespace hod::inline ui2
{
	namespace
	{
		PrefabInstance::SourceProvider sourceProvider = nullptr;

		// The prefabs being loaded on this thread (see PrefabInstance::ScopedLoading). A plain array: a container would be freed at the
		// thread's exit, after the memory manager it allocated from. Nesting deeper than its capacity isn't tracked (a cycle has
		// been caught well before).
		constexpr uint32_t    LoadingPrefabsCapacity = 64;
		thread_local UID      loadingPrefabs[LoadingPrefabsCapacity];
		thread_local uint32_t loadingPrefabCount = 0;

		/// @brief Default SourceProvider: the cooked resource's document.
		bool LoadResourceSource(const UID& prefab, Document& outDocument)
		{
			std::shared_ptr<UIPrefabResource> resource = ResourceManager::GetInstance()->GetResource<UIPrefabResource>(prefab);
			if (resource == nullptr)
			{
				return false;
			}
			outDocument.GetRootNode().Copy(resource->GetDocument().GetRootNode());
			return true;
		}

		bool ContainsImpl(const UID& prefab, const UID& target, Vector<UID>& visited)
		{
			if (prefab == target)
			{
				return true;
			}
			for (const UID& visitedPrefab : visited)
			{
				if (visitedPrefab == prefab)
				{
					return false;
				}
			}
			visited.PushBack(prefab);

			Document document;
			if (PrefabInstance::LoadSource(prefab, document) == false)
			{
				return false;
			}
			Vector<UID> prefabs;
			PrefabInstance::CollectPrefabs(document.GetRootNode(), prefabs);
			for (const UID& nestedPrefab : prefabs)
			{
				if (ContainsImpl(nestedPrefab, target, visited))
				{
					return true;
				}
			}
			return false;
		}
	}

	/// @brief
	/// @param prefab
	PrefabInstance::ScopedLoading::ScopedLoading(const UID& prefab)
	{
		if (loadingPrefabCount < LoadingPrefabsCapacity)
		{
			loadingPrefabs[loadingPrefabCount] = prefab;
		}
		++loadingPrefabCount;
	}

	/// @brief
	PrefabInstance::ScopedLoading::~ScopedLoading()
	{
		--loadingPrefabCount;
	}

	/// @brief
	/// @param provider
	void PrefabInstance::SetSourceProvider(SourceProvider provider)
	{
		sourceProvider = provider;
	}

	/// @brief
	/// @param prefab
	/// @param outDocument
	/// @return
	bool PrefabInstance::LoadSource(const UID& prefab, Document& outDocument)
	{
		return (sourceProvider != nullptr) ? sourceProvider(prefab, outDocument) : LoadResourceSource(prefab, outDocument);
	}

	/// @brief
	/// @param prefab
	/// @return
	bool PrefabInstance::IsLoading(const UID& prefab)
	{
		for (uint32_t index = 0; index < Min(loadingPrefabCount, LoadingPrefabsCapacity); ++index)
		{
			if (loadingPrefabs[index] == prefab)
			{
				return true;
			}
		}
		return false;
	}

	/// @brief
	/// @param entry
	/// @param outPrefabs
	void PrefabInstance::CollectPrefabs(const DocumentNode& entry, Vector<UID>& outPrefabs)
	{
		const DocumentNode* instanceNode = entry.GetChild("PrefabInstance");
		if (instanceNode != nullptr)
		{
			const DocumentNode* prefabNode = instanceNode->GetChild("Prefab");
			if (prefabNode != nullptr && prefabNode->GetType() == DocumentNode::Type::String)
			{
				outPrefabs.PushBack(UID::FromString(prefabNode->GetString().CStr()));
			}
			return; // An instance's entry has no children of its own
		}

		const DocumentNode* childrenNode = entry.GetChild("Children");
		if (childrenNode == nullptr)
		{
			return;
		}
		for (const DocumentNode* childNode = childrenNode->GetFirstChild(); childNode != nullptr; childNode = childNode->GetNextSibling())
		{
			CollectPrefabs(*childNode, outPrefabs);
		}
	}

	/// @brief
	/// @param prefab
	/// @param target
	/// @return
	bool PrefabInstance::Contains(const UID& prefab, const UID& target)
	{
		Vector<UID> visited;
		return ContainsImpl(prefab, target, visited);
	}

	/// @brief
	/// @return
	PrefabInstance::SourceChangedEvent& PrefabInstance::GetSourceChangedEvent()
	{
		static SourceChangedEvent sourceChangedEvent;
		return sourceChangedEvent;
	}

	/// @brief
	/// @param prefab
	PrefabInstance::PrefabInstance(const UID& prefab)
	: _prefab(prefab)
	{
	}

	/// @brief
	PrefabInstance::~PrefabInstance()
	{
		DefaultAllocator::GetInstance().Delete(_sourceContainer);
	}

	/// @brief
	/// @return
	const UID& PrefabInstance::GetPrefab() const
	{
		return _prefab;
	}

	/// @brief
	/// @return
	bool PrefabInstance::IsMissing() const
	{
		return _missing;
	}

	/// @brief
	/// @return
	uint64_t PrefabInstance::GetRootLocalId() const
	{
		return _rootLocalId;
	}

	/// @brief
	/// @param localId
	/// @return
	Node* PrefabInstance::FindSourceNode(uint64_t localId)
	{
		if (_missing)
		{
			return nullptr;
		}

		if (_sourceContainer == nullptr)
		{
			_sourceContainer = DefaultAllocator::GetInstance().New<Node>();
			ScopedLoading loading(_prefab);
			_sourceContainer->AddChildFromDocument(_source.GetRootNode());
		}

		// The source root is a plain node (not an instance): found by its id in the prefab, as the instance root is from inside
		return _sourceContainer->GetChildren().Empty() ? nullptr : _sourceContainer->GetChildren()[0]->FindByLocalId(localId);
	}

	/// @brief
	/// @param instanceRoot
	/// @param outOverrides
	void PrefabInstance::CollectOverrides(Node& instanceRoot, Vector<PropertyOverride>& outOverrides)
	{
		if (_missing)
		{
			outOverrides = _missingOverrides;
			return;
		}

		Vector<Node*> nodes = {&instanceRoot};
		for (uint32_t index = 0; index < nodes.Size(); ++index)
		{
			Node*    node = nodes[index];
			bool     isRoot = (node == &instanceRoot);
			uint64_t localId = isRoot ? _rootLocalId : node->GetLocalId();
			Node*    sourceNode = (localId != 0) ? FindSourceNode(localId) : nullptr;
			if (sourceNode != nullptr)
			{
				for (const String& property : PropertyValue::GetOverridableProperties(*node, true))
				{
					if (isRoot && PropertyValue::IsLayoutParamsProperty(property))
					{
						continue;
					}

					PropertyValue value;
					PropertyValue sourceValue;
					if (value.Read(*node, property) && (sourceValue.Read(*sourceNode, property) == false || value.Equals(sourceValue) == false))
					{
						PropertyOverride& propertyOverride = outOverrides.EmplaceBack();
						propertyOverride._target = localId;
						propertyOverride._property = property;
						propertyOverride.SetValue(value);
					}
				}
			}

			// A nested instance's content is its own prefab's business
			if (isRoot || node->GetPrefabInstance() == nullptr)
			{
				for (Node* child : node->GetChildren())
				{
					nodes.PushBack(child);
				}
			}
		}
	}
}
