#pragma once
#include "HodEngine/RendererEditor/Export.hpp"

#include "HodEngine/Editor/AssetType.hpp"

namespace hod::inline editor
{
	/// @brief The materials, imported from .slang shaders
	class HOD_RENDERER_EDITOR_API MaterialAssetType : public AssetType
	{
		HOD_ASSET_TYPE("Material", "material")

	public:
		Cooker*                     CreateCooker() const override;
		const ReflectionDescriptor* GetCookedResource() const override;
		EditorTab*                  CreateEditorTab(std::shared_ptr<Asset> asset) const override;
		Texture*                    GetIcon() const override;
	};
}
