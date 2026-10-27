#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Cooker/Cooker.hpp"
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/Renderer/Resource/FontResource.hpp"
#include "HodEngine/GameSystems/Resource/WeakResource.hpp"

namespace hod::inline editor
{
	/// @brief How the glyphs of a font are drawn, set on its asset ("Settings" data block), cooked into its FontResource ("Info" data block)
	class HOD_EDITOR_API FontSettings
	{
		REFLECTED_CLASS_NO_VIRTUAL(FontSettings)

	public:
		/// @brief See FontResource::Style
		struct Style
		{
			REFLECTED_CLASS_NO_VIRTUAL(Style)

			WeakResource<FontResource>	_font;
			FontResource::Weight		_weight = FontResource::Weight::Regular;
			bool						_italic = false;
		};

	public:
		FontResource::RenderMode	_renderMode = FontResource::RenderMode::Smooth;
		uint32_t					_pixelSize = 0; // PixelArt only, see FontResource::GetPixelSize
		Vector<WeakResource<FontResource>>	_fallbacks; // see FontResource::GetFallbacks
		Vector<Style>				_styles; // see FontResource::GetStyles

		// The weight and the italic a face says it has (OS/2), the font not set
		static Style				DetectStyle(const FontFace& face);
	};

	class HOD_EDITOR_API FontCooker : public Cooker
	{
	public:

							FontCooker();
							FontCooker(const FontCooker&) = delete;
							FontCooker(FontCooker&&) = delete;
							~FontCooker() override = default;

		FontCooker&		operator = (const FontCooker&) = delete;
		FontCooker&		operator = (FontCooker&&) = delete;

	protected:

		bool FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages) override;
	};
}
