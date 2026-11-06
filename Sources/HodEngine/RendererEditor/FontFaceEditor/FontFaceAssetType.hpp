#pragma once
#include "HodEngine/RendererEditor/Export.hpp"

#include "HodEngine/Editor/AssetType.hpp"

namespace hod::inline editor
{
	/// @brief A style of a font (regular, bold...), imported from .ttf/.otf files
	class HOD_RENDERER_EDITOR_API FontFaceAssetType : public AssetType
	{
		HOD_ASSET_TYPE("FontFace", "fontface")

	public:
		Cooker*                     CreateCooker() const override;
		const ReflectionDescriptor* GetCookedResource() const override;
		EditorTab*                  CreateEditorTab(std::shared_ptr<Asset> asset) const override;
		Texture*                    GetIcon() const override;
	};
}
