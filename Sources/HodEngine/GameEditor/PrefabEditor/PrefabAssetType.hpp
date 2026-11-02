#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/AssetType.hpp"

namespace hod::inline editor
{
	/// @brief The prefabs: entities instantiated in scenes and other prefabs
	class HOD_GAME_EDITOR_API PrefabAssetType : public AssetType
	{
		HOD_ASSET_TYPE("Prefab")

	public:
		Cooker*                     CreateCooker() const override;
		const ReflectionDescriptor* GetCookedResource() const override;
		EditorTab*                  CreateEditorTab(std::shared_ptr<Asset> asset) const override;
		Texture*                    GetIcon() const override;
		void                        OnRegistered(EditorModuleRegistration& registration) override;
	};
}
