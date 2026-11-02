#include "HodEngine/RendererEditor/Pch.hpp"
#include "HodEngine/RendererEditor/FontEditor/FontAssetType.hpp"
#include "HodEngine/RendererEditor/FontEditor/FontCooker.hpp"
#include "HodEngine/RendererEditor/FontEditor/FontEditorTab.hpp"
#include "HodEngine/RendererEditor/FontEditor/FontHelper.hpp"
#include "HodEngine/RendererEditor/FontFaceEditor/FontFaceAssetType.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/EditorModuleRegistration.hpp"

#include <HodEngine/Renderer/Resource/FontResource.hpp>

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @return
	Cooker* FontAssetType::CreateCooker() const
	{
		return DefaultAllocator::GetInstance().New<FontCooker>();
	}

	/// @brief
	/// @return
	const ReflectionDescriptor* FontAssetType::GetCookedResource() const
	{
		return &FontResource::GetReflectionDescriptor();
	}

	/// @brief
	/// @param asset
	/// @return
	EditorTab* FontAssetType::CreateEditorTab(std::shared_ptr<Asset> asset) const
	{
		return DefaultAllocator::GetInstance().New<FontEditorTab>(asset);
	}

	/// @brief
	/// @return
	Texture* FontAssetType::GetIcon() const
	{
		return Editor::GetInstance()->GetFontTexture();
	}

	/// @brief
	/// @param registration
	void FontAssetType::OnRegistered(EditorModuleRegistration& registration)
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
						if (node == nullptr || node->_asset == nullptr || node->_asset->GetType() != FontFaceAssetType::GetHash())
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
