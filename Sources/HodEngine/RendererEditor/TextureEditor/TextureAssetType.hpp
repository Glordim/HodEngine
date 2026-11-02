#pragma once
#include "HodEngine/RendererEditor/Export.hpp"

#include "HodEngine/Editor/AssetType.hpp"

namespace hod::inline editor
{
	/// @brief The textures, imported from image files
	class HOD_RENDERER_EDITOR_API TextureAssetType : public AssetType
	{
		HOD_ASSET_TYPE("Texture")

	public:
		Cooker*                     CreateCooker() const override;
		const ReflectionDescriptor* GetCookedResource() const override;
		EditorTab*                  CreateEditorTab(std::shared_ptr<Asset> asset) const override;
	};
}
