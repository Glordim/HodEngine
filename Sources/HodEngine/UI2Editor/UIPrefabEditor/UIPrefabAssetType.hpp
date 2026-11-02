#pragma once
#include "HodEngine/UI2Editor/Export.hpp"

#include "HodEngine/Editor/AssetType.hpp"

namespace hod::inline editor
{
	/// @brief The UI2 prefabs: nodes edited in a UIPrefabEditorTab, nested in each other as instances
	class HOD_UI2_EDITOR_API UIPrefabAssetType : public AssetType
	{
		HOD_ASSET_TYPE("UIPrefab")

	public:
		Cooker*                     CreateCooker() const override;
		const ReflectionDescriptor* GetCookedResource() const override;
		EditorTab*                  CreateEditorTab(std::shared_ptr<Asset> asset) const override;
		void                        OnRenamed(Asset& asset) const override;
		void                        OnRegistered(EditorModuleRegistration& registration) override;
		void                        OnUnregistered() override;
	};
}
