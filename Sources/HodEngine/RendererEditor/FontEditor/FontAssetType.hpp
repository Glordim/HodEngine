#pragma once
#include "HodEngine/RendererEditor/Export.hpp"

#include "HodEngine/Editor/AssetType.hpp"

namespace hod::inline editor
{
	/// @brief A font family: the faces of its styles, what a text is drawn with
	class HOD_RENDERER_EDITOR_API FontAssetType : public AssetType
	{
		HOD_ASSET_TYPE("Font", "font")

	public:
		Cooker*                     CreateCooker() const override;
		const ReflectionDescriptor* GetCookedResource() const override;
		EditorTab*                  CreateEditorTab(std::shared_ptr<Asset> asset) const override;
		Texture*                    GetIcon() const override;
		void                        OnRegistered(EditorRegistrar& editorRegistrar) override;
	};
}
