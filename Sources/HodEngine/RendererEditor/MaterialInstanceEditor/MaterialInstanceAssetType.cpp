#include "HodEngine/RendererEditor/Pch.hpp"
#include "HodEngine/RendererEditor/MaterialInstanceEditor/MaterialInstanceAssetType.hpp"
#include "HodEngine/RendererEditor/MaterialEditor/MaterialAssetType.hpp"
#include "HodEngine/RendererEditor/MaterialInstanceEditor/MaterialInstanceCooker.hpp"
#include "HodEngine/RendererEditor/MaterialInstanceEditor/MaterialInstanceEditorTab.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/EditorModuleRegistration.hpp"

#include <HodEngine/Renderer/Resource/MaterialInstanceResource.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @return
	Cooker* MaterialInstanceAssetType::CreateCooker() const
	{
		return DefaultAllocator::GetInstance().New<MaterialInstanceCooker>();
	}

	/// @brief
	/// @return
	const ReflectionDescriptor* MaterialInstanceAssetType::GetCookedResource() const
	{
		return &MaterialInstanceResource::GetReflectionDescriptor();
	}

	/// @brief
	/// @param asset
	/// @return
	EditorTab* MaterialInstanceAssetType::CreateEditorTab(std::shared_ptr<Asset> asset) const
	{
		return DefaultAllocator::GetInstance().New<MaterialInstanceEditorTab>(asset);
	}

	/// @brief
	/// @param registration
	void MaterialInstanceAssetType::OnRegistered(EditorModuleRegistration& registration)
	{
		registration.RegisterContextualMenuAction({
			.path = "Create/Material Instance",
			.group = "New",
			.available =
				+[](const AssetBrowserWindow::Context& context)
				{
					if (context.selectedItems.Size() == 1)
					{
						AssetDatabase::FileSystemMapping* asset = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(context.selectedItems[0]);
						if (asset != nullptr && asset->_asset != nullptr)
						{
							return asset->_asset->GetType() == MaterialAssetType::GetHash();
						}
					}
					return false;
				},
			.execute =
				+[](const AssetBrowserWindow::Context& context)
				{
					AssetDatabase::FileSystemMapping* materialNode = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(context.selectedItems[0]);
					if (materialNode == nullptr || materialNode->_asset == nullptr)
					{
						return;
					}

					Path assetPath = AssetDatabase::GenerateUniqueAssetPath(context.currentDirectory / (materialNode->_asset->GetName() + ".materialinstance"));

					MaterialInstanceSettings materialInstanceSettings;
					materialInstanceSettings._material.SetUid(materialNode->_asset->GetUid());

					Document settingsDocument;
					Serializer::Serialize(materialInstanceSettings, settingsDocument.GetRootNode());

					AssetContainer assetContainer;
					assetContainer.SetAssetType(MaterialInstanceAssetType::GetHash());
					assetContainer.SetUid(UID::GenerateUID());

					Stream&            settingsStream = assetContainer.AddDataBlock("Settings", false);
					DocumentWriterJson documentWriter;
					documentWriter.Write(settingsDocument, settingsStream);

					assetContainer.Save(assetPath);

					context.assetBrowserWindow.Rename(assetPath);
				},
		});
	}
}
