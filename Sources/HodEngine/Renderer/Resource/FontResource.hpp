#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/GameSystems/Resource/Resource.hpp"

#include <HodEngine/Core/UID.hpp>
#include <HodEngine/Core/Vector.hpp>

namespace hod::inline renderer
{
	/// @brief A font family, what a text is drawn with: the face drawn for each style (FontFaceResource), and the fonts the characters its faces lack
	/// are drawn with. See FontChain
	class HOD_RENDERER_API FontResource : public Resource
	{
		REFLECTED_CLASS(FontResource, Resource)

	public:

		/// @brief The weights of CSS (and of the OS/2 table of a font file)
		enum class Weight : uint16_t
		{
			Thin = 100,
			ExtraLight = 200,
			Light = 300,
			Regular = 400,
			Medium = 500,
			SemiBold = 600,
			Bold = 700,
			ExtraBold = 800,
			Black = 900,
		};
		REFLECTED_ENUM(HOD_RENDERER_API, Weight);

		/// @brief The face drawn for a text asking for this style
		struct Style
		{
			UID		_face;
			Weight	_weight = Weight::Regular;
			bool	_italic = false;
		};

	public:

							FontResource() = default;
							FontResource(const FontResource&) = delete;
							FontResource(FontResource&&) = delete;
							~FontResource() override = default;

		FontResource&		operator = (const FontResource&) = delete;
		FontResource&		operator = (FontResource&&) = delete;

	public:

		bool				Initialize(const ResourceContainer& resourceContainer) override;

		// The face drawn for each style, the same one for several maybe. None: nothing is drawn with the font
		const Vector<Style>&	GetStyles() const;
		void				SetStyles(const Vector<Style>& styles);

		// The face of its styles closest to a style, as CSS matches it: the italic first, then the weight. An invalid UID without styles
		UID					FindStyle(Weight weight, bool italic) const;

		// The fonts the characters its faces lack are drawn with, in the order they are looked into (their own fallbacks after each of them),
		// each in the style asked for
		const Vector<UID>&	GetFallbacks() const;
		void				SetFallbacks(const Vector<UID>& fallbacks);

	private:

		Vector<Style>		_styles;    // not locked here: only the faces drawn are loaded
		Vector<UID>			_fallbacks; // nor here: fonts falling back on each other would keep each other alive
	};
}
