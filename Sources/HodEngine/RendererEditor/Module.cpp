#include "HodEngine/RendererEditor/Pch.hpp"
#include "HodEngine/RendererEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/EditorModuleRegistration.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"

#include "HodEngine/RendererEditor/FontEditor/FontCooker.hpp"
#include "HodEngine/RendererEditor/FontEditor/FontEditorTab.hpp"
#include "HodEngine/RendererEditor/FontEditor/FontHelper.hpp"
#include "HodEngine/RendererEditor/FontFaceEditor/FontFaceCooker.hpp"
#include "HodEngine/RendererEditor/FontFaceEditor/FontFaceEditorTab.hpp"
#include "HodEngine/RendererEditor/FontFaceEditor/FontFaceImporter.hpp"
#include "HodEngine/RendererEditor/MaterialEditor/MaterialCooker.hpp"
#include "HodEngine/RendererEditor/MaterialEditor/MaterialEditorTab.hpp"
#include "HodEngine/RendererEditor/MaterialEditor/MaterialImporter.hpp"
#include "HodEngine/RendererEditor/MaterialInstanceEditor/MaterialInstanceCooker.hpp"
#include "HodEngine/RendererEditor/MaterialInstanceEditor/MaterialInstanceEditorTab.hpp"
#include "HodEngine/RendererEditor/TextureEditor/TextureCooker.hpp"
#include "HodEngine/RendererEditor/TextureEditor/TextureEditorTab.hpp"
#include "HodEngine/RendererEditor/TextureEditor/TextureImporter.hpp"

#include <HodEngine/Renderer/Resource/FontFaceResource.hpp>
#include <HodEngine/Renderer/Resource/FontResource.hpp>
#include <HodEngine/Renderer/Resource/MaterialInstanceResource.hpp>
#include <HodEngine/Renderer/Resource/MaterialResource.hpp>
#include <HodEngine/Renderer/Resource/TextureResource.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Hash.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::editor;

namespace
{
	EditorModuleRegistration registration;

	/// @brief "Create/Material Instance": of the selected material
	void RegisterCreateMaterialInstance()
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
							return asset->_asset->GetType() == Hash::ComputeXxh3_64("Material");
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
					assetContainer.SetAssetType(Hash::ComputeXxh3_64("MaterialInstance"));
					assetContainer.SetUid(UID::GenerateUID());

					Stream&            settingsStream = assetContainer.AddDataBlock("Settings", false);
					DocumentWriterJson documentWriter;
					documentWriter.Write(settingsDocument, settingsStream);

					assetContainer.Save(assetPath);

					context.assetBrowserWindow.Rename(assetPath);
				},
		});
	}

	/// @brief "Create/Font": from the selected faces
	void RegisterCreateFont()
	{
		registration.RegisterContextualMenuAction({
			.path = "Create/Font",
			.group = "New",
			.available =
				+[](const AssetBrowserWindow::Context& context)
				{
					// From faces: a text is drawn with a font, the family they are styles of
					for (const Path& selectedItem : context.selectedItems)
					{
						AssetDatabase::FileSystemMapping* node = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(selectedItem);
						if (node == nullptr || node->_asset == nullptr || node->_asset->GetType() != Hash::ComputeXxh3_64("FontFace"))
						{
							return false;
						}
					}
					return context.selectedItems.Empty() == false;
				},
			.execute =
				+[](const AssetBrowserWindow::Context& context)
				{
					Vector<std::shared_ptr<Asset>> faceAssets;
					for (const Path& selectedItem : context.selectedItems)
					{
						AssetDatabase::FileSystemMapping* node = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(selectedItem);
						if (node != nullptr && node->_asset != nullptr)
						{
							faceAssets.PushBack(node->_asset);
						}
					}

					// Its styles detected from the faces, the ones of their family in their folder with them
					const Path assetPath = FontHelper::CreateFont(faceAssets, context.currentDirectory);
					if (assetPath.Empty() == false)
					{
						context.assetBrowserWindow.Rename(assetPath);
					}
				},
		});
	}
}

HOD_STARTUP_MODULE(RendererEditor)
{
	registration.RegisterImporter<TextureImporter>("png", "tga", "jpg", "bmp", "psd", "gif", "hdr", "pic");
	registration.RegisterCooker<TextureCooker>("Texture");
	registration.RegisterResource<TextureResource>("Texture");
	registration.RegisterEditorTab<TextureEditorTab>("Texture");

	registration.RegisterImporter<FontFaceImporter>("ttf", "otf");
	registration.RegisterCooker<FontFaceCooker>("FontFace");
	registration.RegisterResource<FontFaceResource>("FontFace");
	registration.RegisterEditorTab<FontFaceEditorTab>("FontFace");

	registration.RegisterCooker<FontCooker>("Font");
	registration.RegisterResource<FontResource>("Font");
	registration.RegisterEditorTab<FontEditorTab>("Font");

	registration.RegisterImporter<MaterialImporter>("slang");
	registration.RegisterCooker<MaterialCooker>("Material");
	registration.RegisterResource<MaterialResource>("Material");
	registration.RegisterEditorTab<MaterialEditorTab>("Material");

	registration.RegisterCooker<MaterialInstanceCooker>("MaterialInstance");
	registration.RegisterResource<MaterialInstanceResource>("MaterialInstance");
	registration.RegisterEditorTab<MaterialInstanceEditorTab>("MaterialInstance");

	RegisterCreateMaterialInstance();
	RegisterCreateFont();

	return 0;
}

HOD_SHUTDOWN_MODULE(RendererEditor)
{
	registration.UnregisterAll();
	return 0;
}
