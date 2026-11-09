#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/PrefabEditor/PrefabAssetType.hpp"
#include "HodEngine/GameEditor/PrefabEditor/PrefabCooker.hpp"
#include "HodEngine/GameEditor/PrefabEditor/PrefabEditorTab.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/EditorModuleRegistration.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"

#include <HodEngine/Game/PrefabResource.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/UID.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @return
	Cooker* PrefabAssetType::CreateCooker() const
	{
		return DefaultAllocator::GetInstance().New<PrefabCooker>();
	}

	/// @brief
	/// @return
	const ReflectionDescriptor* PrefabAssetType::GetCookedResource() const
	{
		return &PrefabResource::GetReflectionDescriptor();
	}

	/// @brief
	/// @param asset
	/// @return
	EditorTab* PrefabAssetType::CreateEditorTab(std::shared_ptr<Asset> asset) const
	{
		return DefaultAllocator::GetInstance().New<PrefabEditorTab>(asset);
	}

	/// @brief
	/// @return
	Texture* PrefabAssetType::GetIcon() const
	{
		return Editor::GetInstance()->GetPrefabTexture();
	}

	/// @brief A prefab's root entity is named after its asset (see PrefabEditorTab): renamed in its file along with it. Its instances follow
	/// once it is cooked again (with their name as an override only when given one of their own); an open tab of it follows on its own.
	/// @param asset
	void PrefabAssetType::OnRenamed(Asset& asset) const
	{
		AssetContainer assetContainer;
		if (assetContainer.Load(asset.GetPath()) == false)
		{
			return;
		}
		const AssetContainer::DataBlockInfo* entitiesDataBlock = assetContainer.FindDataBlock("Entities");
		Document                             document;
		DocumentReaderJson                   documentReader;
		if (entitiesDataBlock == nullptr || documentReader.Read(document, *entitiesDataBlock->_stream) == false)
		{
			OUTPUT_ERROR("Can't read the prefab {} to name its root entity after it", asset.GetPath());
			return;
		}
		if (PrefabEditorTab::NameRootEntityAfterAsset(document, asset.GetName()) == false)
		{
			return; // Named so already (or not a single root)
		}

		DocumentWriterJson documentWriter;
		if (documentWriter.Write(document, assetContainer.AddDataBlock("Entities", false)) == false || assetContainer.Save(asset.GetPath()) == false)
		{
			OUTPUT_ERROR("Can't write the prefab {} to name its root entity after it", asset.GetPath());
		}
	}

	/// @brief
	/// @param registration
	void PrefabAssetType::OnRegistered(EditorModuleRegistration& registration)
	{
		// "Create/Prefab": an empty one
		registration.RegisterContextualMenuAction({
			.path = "Create/Prefab",
			.group = "New",
			.available = +[](const AssetBrowserWindow::Context& context) { return context.selectedItems.Empty(); },
			.execute =
				+[](const AssetBrowserWindow::Context& context)
				{
					// With its root already (see PrefabEditorTab), named after the asset: renamed along with it
					Path assetPath = AssetDatabase::GenerateUniqueAssetPath(context.currentDirectory / GetFileName("NewPrefab"));
					if (PrefabEditorTab::CreateEmptyPrefab(assetPath) == false)
					{
						OUTPUT_ERROR("Can't write the prefab {}", assetPath);
						return;
					}

					context.assetBrowserWindow.Rename(assetPath);
				},
		});
	}
}
