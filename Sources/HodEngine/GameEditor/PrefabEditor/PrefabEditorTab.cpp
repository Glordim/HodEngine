#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/PrefabEditor/PrefabEditorTab.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/HierachyWindow.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/InspectorWindow.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/ViewportWindow.hpp"
#include "HodEngine/GameEditor/PrefabEditor/PrefabAssetType.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/PrefabResource.hpp>
#include <HodEngine/Game/Scene.hpp>
#include <HodEngine/Game/SceneSerializer.hpp>
#include <HodEngine/Game/World.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/Core/UID.hpp>
#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>

#include <algorithm>

namespace hod::inline editor
{
	/// @brief
	PrefabEditorTab::PrefabEditorTab(std::shared_ptr<Asset> asset)
	: EntityEditorTab(asset, ICON_MDI_CUBE)
	{
		if (asset != nullptr)
		{
			_scene->SetName(asset->GetName());

			// The asset as saved (as the scene tab loads its scene), not its cooked resource: the root stays plain entities, not an instance
			const AssetContainer::DataBlockInfo* entitiesDataBlock = _assetContainer.FindDataBlock("Entities");
			if (entitiesDataBlock != nullptr)
			{
				Document           document;
				DocumentReaderJson documentReader;
				if (documentReader.Read(document, *entitiesDataBlock->_stream) == false || Serializer::Deserialize(_scene, document.GetRootNode()) == false)
				{
					OUTPUT_ERROR("Can't read the prefab {}", asset->GetPath());
				}
				_scene->SetName(asset->GetName());
			}
		}

		SyncRootEntityName();
		_scene->ProcessActivation();
	}

	/// @brief
	PrefabEditorTab::~PrefabEditorTab() {}

	/// @brief The entity without parent: a prefab has one (see CreateEmptyPrefab)
	/// @return
	Entity* PrefabEditorTab::GetRootEntity() const
	{
		for (const auto& entityPair : _scene->GetEntities())
		{
			if (entityPair.second->GetParent().Lock() == nullptr)
			{
				return entityPair.second;
			}
		}
		return nullptr;
	}

	/// @brief Not itself, nor a prefab holding an instance of it: it would contain itself
	/// @param prefab
	/// @return
	bool PrefabEditorTab::CanInstantiatePrefab(const UID& prefab) const
	{
		std::shared_ptr<Asset> asset = GetAsset();
		if (asset == nullptr)
		{
			return true;
		}
		return prefab != asset->GetUid() && ContainsPrefab(prefab, asset->GetUid()) == false;
	}

	/// @brief A plain entity with a Node2dComponent: what places the instances in the scenes
	/// @param name
	/// @return
	Entity* PrefabEditorTab::CreateRootEntity(std::string_view name)
	{
		Entity* rootEntity = DefaultAllocator::GetInstance().New<Entity>(name);
		rootEntity->AddComponent<Node2dComponent>();
		return rootEntity;
	}

	/// @brief
	void PrefabEditorTab::SyncRootEntityName()
	{
		std::shared_ptr<Asset> asset = GetAsset();
		Entity*                rootEntity = GetRootEntity();
		if (asset == nullptr || rootEntity == nullptr)
		{
			return;
		}
		if (rootEntity->GetName() != asset->GetName())
		{
			rootEntity->SetName(asset->GetName());
		}
		if (_scene->GetName() != asset->GetName())
		{
			_scene->SetName(asset->GetName());
		}
	}

	/// @brief
	/// @param assetPath
	/// @return
	bool PrefabEditorTab::CreateEmptyPrefab(const Path& assetPath)
	{
		String  name = assetPath.Stem().GetString();
		Entity* rootEntity = CreateRootEntity(name);

		// As Scene::SerializeInDocument writes it
		Document        document;
		uint64_t        nextLocalId = 1;
		SceneSerializer sceneSerializer;
		document.GetRootNode().AddChild("Name").SetString(name);
		bool serialized = sceneSerializer.SerializeEntity(rootEntity, true, document.GetRootNode().AddChild("Entities"), nextLocalId);
		document.GetRootNode().AddChild("NextLocalId").SetUInt64(nextLocalId);
		DefaultAllocator::GetInstance().Delete(rootEntity);
		if (serialized == false)
		{
			return false;
		}

		AssetContainer     assetContainer;
		DocumentWriterJson documentWriter;
		assetContainer.SetAssetType(PrefabAssetType::GetHash());
		assetContainer.SetUid(UID::GenerateUID());
		return documentWriter.Write(document, assetContainer.AddDataBlock("Entities", false)) && assetContainer.Save(assetPath);
	}

	/// @brief The entities are written roots first (see SceneSerializer::SerializeEntity), the root is the only one without parent
	/// @param prefabDocument
	/// @return nullptr if there isn't exactly one
	const DocumentNode* PrefabEditorTab::FindRootEntityNode(const Document& prefabDocument)
	{
		const DocumentNode* entitiesNode = prefabDocument.GetRootNode().GetChild("Entities");
		if (entitiesNode == nullptr)
		{
			return nullptr;
		}

		const DocumentNode* rootNode = nullptr;
		for (const DocumentNode* entityNode = entitiesNode->GetFirstChild(); entityNode != nullptr; entityNode = entityNode->GetNextSibling())
		{
			const DocumentNode* parentNode = entityNode->GetChild("Parent");
			const DocumentNode* parentIdNode = parentNode != nullptr ? parentNode->GetChild("Id") : nullptr;
			if (parentIdNode == nullptr || parentIdNode->GetUInt64() == 0)
			{
				if (rootNode != nullptr)
				{
					return nullptr;
				}
				rootNode = entityNode;
			}
		}
		return rootNode;
	}

	/// @brief
	/// @param prefabDocument
	/// @param assetName
	/// @return
	bool PrefabEditorTab::NameRootEntityAfterAsset(Document& prefabDocument, std::string_view assetName)
	{
		DocumentNode* rootNode = const_cast<DocumentNode*>(FindRootEntityNode(prefabDocument));
		if (rootNode == nullptr || rootNode->GetChild("PrefabInstance") != nullptr)
		{
			return false;
		}

		bool          changed = false;
		DocumentNode& nameNode = rootNode->GetOrAddChild("_name");
		if (nameNode.GetType() != DocumentNode::Type::String || std::string_view(nameNode.GetString().CStr(), nameNode.GetString().Length()) != assetName)
		{
			nameNode.SetString(assetName);
			changed = true;
		}
		DocumentNode& sceneNameNode = prefabDocument.GetRootNode().GetOrAddChild("Name");
		if (sceneNameNode.GetType() != DocumentNode::Type::String || std::string_view(sceneNameNode.GetString().CStr(), sceneNameNode.GetString().Length()) != assetName)
		{
			sceneNameNode.SetString(assetName);
			changed = true;
		}
		return changed;
	}

	/// @brief Through the cooked resources' documents: the instances they hold, then theirs
	/// @param prefab
	/// @param other
	/// @return
	bool PrefabEditorTab::ContainsPrefab(const UID& prefab, const UID& other)
	{
		Vector<UID> visited;
		Vector<UID> pending;
		pending.PushBack(prefab);
		while (pending.Empty() == false)
		{
			UID current = pending[pending.Size() - 1];
			pending.PopBack();
			if (std::find(visited.Begin(), visited.End(), current) != visited.End())
			{
				continue;
			}
			visited.PushBack(current);

			std::shared_ptr<PrefabResource> prefabResource = ResourceManager::GetInstance()->GetResource<PrefabResource>(current);
			if (prefabResource == nullptr)
			{
				continue;
			}
			const DocumentNode* entitiesNode = prefabResource->GetDocument().GetRootNode().GetChild("Entities");
			if (entitiesNode == nullptr)
			{
				continue;
			}
			for (const DocumentNode* entityNode = entitiesNode->GetFirstChild(); entityNode != nullptr; entityNode = entityNode->GetNextSibling())
			{
				const DocumentNode* prefabInstanceNode = entityNode->GetChild("PrefabInstance");
				const DocumentNode* uidNode = prefabInstanceNode != nullptr ? prefabInstanceNode->GetChild("UID") : nullptr;
				if (uidNode == nullptr)
				{
					continue;
				}
				UID instantiated;
				Serializer::Deserialize(instantiated, *uidNode);
				if (instantiated == other)
				{
					return true;
				}
				pending.PushBack(instantiated);
			}
		}
		return false;
	}

	/// @brief
	/// @return
	bool PrefabEditorTab::OnSave()
	{
		std::shared_ptr<Asset> asset = GetAsset();
		if (asset == nullptr)
		{
			return false;
		}

		_assetContainer.ClearDataBlocks();
		Stream& entitiesStream = _assetContainer.AddDataBlock("Entities", false);

		Document document;
		if (Serializer::Serialize(_scene, document.GetRootNode()) == false)
		{
			return false; // todo message + bool
		}

		DocumentWriterJson documentwriter;
		if (documentwriter.Write(document, entitiesStream) == false)
		{
			return false; // todo message + bool
		}

		return true;
	}

	/// @brief
	/// @param document
	/// @return
	bool PrefabEditorTab::RestoreUndoState(const Document& document)
	{
		bool restored = EntityEditorTab::RestoreUndoState(document);
		SyncRootEntityName(); // A step from before a rename of the asset
		return restored;
	}

	/// @brief
	void PrefabEditorTab::CreateDefaultLayout()
	{
		HierachyWindow*  hierarchyWindow = OpenWindow<HierachyWindow>();
		InspectorWindow* inspectorWindow = OpenWindow<InspectorWindow>();
		ViewportWindow*  viewportWindow = OpenWindow<ViewportWindow>();

		ImGui::DockBuilderRemoveNode(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeChildNodes(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeDockedWindows(_dockSpaceId);
		ImGui::DockBuilderAddNode(_dockSpaceId);
		ImGui::DockBuilderSetNodeSize(_dockSpaceId, ImGui::GetIO().DisplaySize);

		ImGuiID opposite = _dockSpaceId;
		ImGuiID dockLeft = ImGui::DockBuilderSplitNode(_dockSpaceId, ImGuiDir_Left, 0.2f, nullptr, &opposite);
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(opposite, ImGuiDir_Right, 0.2f, nullptr, &opposite);

		ImGui::DockBuilderDockWindow(hierarchyWindow->GetIdentifier(), dockLeft);
		ImGui::DockBuilderDockWindow(inspectorWindow->GetIdentifier(), dockRight);
		ImGui::DockBuilderDockWindow(viewportWindow->GetIdentifier(), opposite);
		ImGuiDockNode* viewportNode = ImGui::DockBuilderGetNode(opposite);
		viewportNode->SetLocalFlags(viewportNode->LocalFlags | ImGuiDockNodeFlags_HiddenTabBar);
		ImGui::DockBuilderFinish(_dockSpaceId);
	}

	/// @brief
	/// @return
	bool PrefabEditorTab::DrawContent()
	{
		SyncRootEntityName(); // The asset may have been renamed since the last frame
		return true;
	}
}
