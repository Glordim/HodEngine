#pragma once
#include "HodEngine/UIEditor/Export.hpp"

#include "HodEngine/Editor/AssetType.hpp"

namespace hod::inline editor
{
	/// @brief The UI prefabs: nodes edited in a UIPrefabEditorTab, nested in each other as instances
	class HOD_UI_EDITOR_API UIPrefabAssetType : public AssetType
	{
		HOD_ASSET_TYPE("UIPrefab", "uiprefab")

	public:
		Cooker*                     CreateCooker() const override;
		const ReflectionDescriptor* GetCookedResource() const override;
		EditorTab*                  CreateEditorTab(std::shared_ptr<Asset> asset) const override;
		void                        OnRenamed(Asset& asset) const override;
		void                        OnRegistered(EditorModuleRegistration& registration) override;
		void                        OnUnregistered() override;
	};
}
