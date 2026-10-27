#pragma once
#include "HodEngine/RendererEditor/Export.hpp"

#include "HodEngine/Editor/Cooker/Cooker.hpp"
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/Renderer/Resource/FontFaceResource.hpp"
#include "HodEngine/Renderer/Resource/FontResource.hpp"
#include "HodEngine/GameSystems/Resource/WeakResource.hpp"

namespace hod::inline editor
{
	/// @brief The family of a font: its faces for each style and its fallbacks, set on its asset ("Settings" data block), cooked into its FontResource
	/// ("Info" data block)
	class HOD_RENDERER_EDITOR_API FontSettings
	{
		REFLECTED_CLASS_NO_VIRTUAL(FontSettings)

	public:
		/// @brief See FontResource::Style
		struct Style
		{
			REFLECTED_CLASS_NO_VIRTUAL(Style)

			WeakResource<FontFaceResource>	_face;
			FontResource::Weight			_weight = FontResource::Weight::Regular;
			bool							_italic = false;
		};

	public:
		// The ones set: the ones the resource has
		Vector<FontResource::Style>			GetStyles() const;
		Vector<UID>							GetFallbacks(const UID& font) const; // but the font itself

	public:
		Vector<Style>						_styles;
		Vector<WeakResource<FontResource>>	_fallbacks; // see FontResource::GetFallbacks
	};

	class HOD_RENDERER_EDITOR_API FontCooker : public Cooker
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
