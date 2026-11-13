#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/AssetType.hpp"

namespace hod::inline editor
{
	/// @brief The scenes: entities edited in an EntityEditorTab, one of them the project's startup scene
	class HOD_GAME_EDITOR_API SceneAssetType : public AssetType
	{
		HOD_ASSET_TYPE("Scene", "scene")

	public:
		Cooker*                     CreateCooker() const override;
		const ReflectionDescriptor* GetCookedResource() const override;
		EditorTab*                  CreateEditorTab(std::shared_ptr<Asset> asset) const override;
		Texture*                    GetIcon() const override;
		void                        OnRegistered(EditorRegistrar& editorRegistrar) override;
	};
}
