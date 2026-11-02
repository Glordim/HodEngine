#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/PrefabEditor/PrefabAssetType.hpp"
#include "HodEngine/GameEditor/PrefabEditor/PrefabCooker.hpp"
#include "HodEngine/GameEditor/PrefabEditor/PrefabEditorTab.hpp"

#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/EditorModuleRegistration.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"

#include <HodEngine/Game/PrefabResource.hpp>

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
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
					Path assetPath = AssetDatabase::GenerateUniqueAssetPath(context.currentDirectory / "NewPrefab.prefab");

					AssetContainer assetContainer;
					assetContainer.SetAssetType(GetHash());
					assetContainer.SetUid(UID::GenerateUID());
					assetContainer.Save(assetPath);

					context.assetBrowserWindow.Rename(assetPath);
				},
		});
	}
}
