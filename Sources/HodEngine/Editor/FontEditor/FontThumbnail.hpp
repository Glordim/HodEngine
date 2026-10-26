#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Renderer/Resource/FontResource.hpp>

#include <cstdint>

namespace hod::inline text
{
	class FontFace;
}

namespace hod::inline editor
{
	/// @brief The thumbnail of a font asset: "Aa", or the letters of the script it is for (see GetSampleText), light on dark, as the font is drawn with its settings.
	/// On the CPU (TextRasterizer): the importer makes it off the main thread
	class HOD_EDITOR_API FontThumbnail
	{
	public:

		static constexpr uint32_t Size = 256;

	public:

		// rgbaPixels: Size x Size, opaque. False when the font has nothing to show
		static bool Generate(const FontFace& face, FontResource::RenderMode renderMode, uint32_t pixelSize, Vector<uint8_t>& rgbaPixels);
	};
}
