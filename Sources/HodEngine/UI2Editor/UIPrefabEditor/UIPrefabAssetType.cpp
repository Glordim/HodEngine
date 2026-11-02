#include "HodEngine/UI2Editor/Pch.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabAssetType.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabCooker.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/EditorModuleRegistration.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"

#include <HodEngine/UI2/PrefabInstance.hpp>
#include <HodEngine/UI2/UIPrefabResource.hpp>

#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::inline editor
{
	/// @brief The nested prefabs' instances are built from the UI prefab assets as last saved, rather than from the cooked
	/// resources (see ui2::PrefabInstance::SourceProvider): a resource still in use isn't reloaded when its asset changes.
	static bool LoadUIPrefabAsset(const UID& prefab, Document& outDocument)
	{
		std::shared_ptr<Asset> asset = AssetDatabase::GetInstance()->Find(prefab);
		if (asset == nullptr || asset->GetType() != UIPrefabAssetType::GetHash())
		{
			return false;
		}

		AssetContainer assetContainer;
		if (assetContainer.Load(asset->GetPath()) == false)
		{
			return false;
		}
		const AssetContainer::DataBlockInfo* nodesDataBlock = assetContainer.FindDataBlock("Nodes");
		if (nodesDataBlock == nullptr)
		{
			return false;
		}

		DocumentReaderJson documentReader;
		if (documentReader.Read(outDocument, *nodesDataBlock->_stream) == false)
		{
			return false;
		}
		// A prefab renamed outside of the editor, or saved before its root was named after it, isn't renamed in its file yet
		UIPrefabEditorTab::NamePrefabRootAfterAsset(outDocument, asset->GetName());
		return true;
	}

	/// @brief
	/// @return
	Cooker* UIPrefabAssetType::CreateCooker() const
	{
		return DefaultAllocator::GetInstance().New<UIPrefabCooker>();
	}

	/// @brief
	/// @return
	const ReflectionDescriptor* UIPrefabAssetType::GetCookedResource() const
	{
		return &UIPrefabResource::GetReflectionDescriptor();
	}

	/// @brief
	/// @param asset
	/// @return
	EditorTab* UIPrefabAssetType::CreateEditorTab(std::shared_ptr<Asset> asset) const
	{
		return DefaultAllocator::GetInstance().New<UIPrefabEditorTab>(asset);
	}

	/// @brief A UI prefab's root is named after its asset (see UIPrefabEditorTab::CanRename): renamed in its file along with it,
	/// its instances following (with their name as an override only when given one of their own)
	/// @param asset
	void UIPrefabAssetType::OnRenamed(Asset& asset) const
	{
		AssetContainer assetContainer;
		if (assetContainer.Load(asset.GetPath()) == false)
		{
			return;
		}
		const AssetContainer::DataBlockInfo* nodesDataBlock = assetContainer.FindDataBlock("Nodes");
		Document                             document;
		DocumentReaderJson                   documentReader;
		if (nodesDataBlock == nullptr || documentReader.Read(document, *nodesDataBlock->_stream) == false)
		{
			OUTPUT_ERROR("Can't read the UI prefab {} to name its root after it", asset.GetPath());
			return;
		}
		if (UIPrefabEditorTab::NamePrefabRootAfterAsset(document, asset.GetName()) == false)
		{
			return;
		}

		DocumentWriterJson documentWriter;
		if (documentWriter.Write(document, assetContainer.AddDataBlock("Nodes", false)) == false || assetContainer.Save(asset.GetPath()) == false)
		{
			OUTPUT_ERROR("Can't write the UI prefab {} to name its root after it", asset.GetPath());
			return;
		}
		// The open tabs' instances of it are rebuilt as after a save (an open tab of the prefab itself follows on its own)
		UIPrefabEditorTab::NotifyPrefabWritten(asset.GetUid());
	}

	/// @brief
	/// @param registration
	void UIPrefabAssetType::OnRegistered(EditorModuleRegistration& registration)
	{
		ui2::PrefabInstance::SetSourceProvider(&LoadUIPrefabAsset); // Reset by OnUnregistered

		registration.RegisterContextualMenuAction({
			.path = "Create/UI Prefab",
			.group = "New",
			.available = +[](const AssetBrowserWindow::Context& context) { return context.selectedItems.Empty(); },
			.execute =
				+[](const AssetBrowserWindow::Context& context)
				{
					// With its root and thumbnail already: not an empty thumbnail until the prefab is first saved
					Path assetPath = AssetDatabase::GenerateUniqueAssetPath(context.currentDirectory / "NewUIPrefab.uiprefab");
					if (UIPrefabEditorTab::CreateEmptyPrefab(assetPath) == false)
					{
						OUTPUT_ERROR("Can't write the UI prefab {}", assetPath);
						return;
					}

					context.assetBrowserWindow.Rename(assetPath);
				},
		});

		// A node dragged from a UI prefab's hierarchy becomes a UI prefab of its own, the node an instance of it
		registration.RegisterDropAction({
			.payloadType = "UI2Node",
			.available =
				+[](const void* payloadData)
				{
					ui2::Node*         node = *static_cast<ui2::Node* const*>(payloadData);
					UIPrefabEditorTab* tab = UIPrefabEditorTab::FindTabOf(node);
					return tab != nullptr && UIPrefabEditorTab::GetNodeRole(node) == UIPrefabEditorTab::NodeRole::Authored && tab->IsRecording() == false &&
					       tab->IsPlaying() == false;
				},
			.execute =
				+[](const void* payloadData, const Path& folder, AssetBrowserWindow& assetBrowserWindow)
				{
					ui2::Node*         node = *static_cast<ui2::Node* const*>(payloadData);
					UIPrefabEditorTab* tab = UIPrefabEditorTab::FindTabOf(node);
					if (tab == nullptr)
					{
						return;
					}
					std::shared_ptr<Asset> asset = tab->CreatePrefabFromNode(node, folder);
					if (asset != nullptr)
					{
						assetBrowserWindow.PingAsset(*asset);
					}
				},
		});
	}

	/// @brief
	void UIPrefabAssetType::OnUnregistered()
	{
		ui2::PrefabInstance::SetSourceProvider(nullptr); // Its code goes with the module
	}
}
