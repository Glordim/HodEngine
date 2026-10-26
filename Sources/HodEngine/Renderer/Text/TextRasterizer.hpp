#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/Text/GlyphCache.hpp"

#include <HodEngine/Core/StringView.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <cstdint>

namespace hod::inline text
{
	class FontFace;
}

namespace hod::inline renderer
{
	/// @brief A line of text rasterized on the CPU, without the GPU nor the atlas of the font: for an importer (a thumbnail...), off the main thread.
	/// Shaped and rasterized as a TextNode draws it: Bitmap glyphs at the size rounded to a pixel, or Pixel glyphs on the grid of the font.
	class HOD_RENDERER_API TextRasterizer final
	{
	public:

		// The bounds of the ink of the line, rows from the top
		struct Image
		{
			uint32_t		_width = 0;
			uint32_t		_height = 0;
			Vector<uint8_t>	_coverage; // _width * _height, 0 to 255
		};

	public:

		// mode: Bitmap, or Pixel with the grid of the font in pixelSize and the size a whole multiple of it. False when nothing is drawn
		static bool Rasterize(const FontFace& face, StringView text, float size, GlyphCache::Mode mode, uint32_t pixelSize, Image& image);
	};
}
