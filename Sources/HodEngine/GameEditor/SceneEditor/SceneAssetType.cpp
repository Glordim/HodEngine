#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/SceneEditor/SceneAssetType.hpp"
#include "HodEngine/GameEditor/SceneEditor/SceneCooker.hpp"
#include "HodEngine/GameEditor/SceneEditor/SceneEditorTab.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/EditorModuleRegistration.hpp"
#include "HodEngine/Editor/Project.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"

#include <HodEngine/Game/SceneResource.hpp>

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/UID.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @return
	Cooker* SceneAssetType::CreateCooker() const
	{
		return DefaultAllocator::GetInstance().New<SceneCooker>();
	}

	/// @brief
	/// @return
	const ReflectionDescriptor* SceneAssetType::GetCookedResource() const
	{
		return &SceneResource::GetReflectionDescriptor();
	}

	/// @brief
	/// @param asset
	/// @return
	EditorTab* SceneAssetType::CreateEditorTab(std::shared_ptr<Asset> asset) const
	{
		return DefaultAllocator::GetInstance().New<SceneEditorTab>(asset);
	}

	/// @brief
	/// @return
	Texture* SceneAssetType::GetIcon() const
	{
		return Editor::GetInstance()->GetSceneTexture();
	}

	/// @brief
	/// @param registration
	void SceneAssetType::OnRegistered(EditorModuleRegistration& registration)
	{
		// "Create/Scene": an empty one
		registration.RegisterContextualMenuAction({
			.path = "Create/Scene",
			.group = "New",
			.available = +[](const AssetBrowserWindow::Context& context) { return context.selectedItems.Empty(); },
			.execute =
				+[](const AssetBrowserWindow::Context& context)
				{
					Path assetPath = AssetDatabase::GenerateUniqueAssetPath(context.currentDirectory / "NewScene.scene");

					AssetContainer assetContainer;
					assetContainer.SetAssetType(GetHash());
					assetContainer.SetUid(UID::GenerateUID());
					assetContainer.Save(assetPath);

					context.assetBrowserWindow.Rename(assetPath);
				},
		});

		// "Set as startup scene" of the project
		registration.RegisterContextualMenuAction({
			.path = "Set as startup scene",
			.group = "",
			.available =
				+[](const AssetBrowserWindow::Context& context)
				{
					if (context.selectedItems.Size() == 1)
					{
						AssetDatabase::FileSystemMapping* asset = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(context.selectedItems[0]);
						if (asset != nullptr && asset->_asset != nullptr)
						{
							return asset->_asset->GetType() == SceneAssetType::GetHash();
						}
					}
					return false;
				},
			.execute =
				+[](const AssetBrowserWindow::Context& context)
				{
					AssetDatabase::FileSystemMapping* asset = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(context.selectedItems[0]);
					if (asset != nullptr && asset->_asset != nullptr)
					{
						Project::GetInstance()->SetStartupScene(asset->_asset);
					}
				},
		});
	}
}
