#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Cooker/Cooker.hpp"
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/Renderer/Resource/FontFaceResource.hpp"

namespace hod::inline editor
{
	/// @brief How the glyphs of a face are drawn, set on its asset ("Settings" data block), cooked into its FontFaceResource ("Info" data block)
	class HOD_EDITOR_API FontFaceSettings
	{
		REFLECTED_CLASS_NO_VIRTUAL(FontFaceSettings)

	public:
		FontFaceResource::RenderMode	_renderMode = FontFaceResource::RenderMode::Smooth;
		uint32_t						_pixelSize = 0; // PixelArt only, see FontFaceResource::GetPixelSize
	};

	class HOD_EDITOR_API FontFaceCooker : public Cooker
	{
	public:

							FontFaceCooker();
							FontFaceCooker(const FontFaceCooker&) = delete;
							FontFaceCooker(FontFaceCooker&&) = delete;
							~FontFaceCooker() override = default;

		FontFaceCooker&		operator = (const FontFaceCooker&) = delete;
		FontFaceCooker&		operator = (FontFaceCooker&&) = delete;

	protected:

		bool FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages) override;
	};
}
