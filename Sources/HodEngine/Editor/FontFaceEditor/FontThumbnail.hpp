#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Renderer/Resource/FontFaceResource.hpp>

#include <cstdint>

namespace hod::inline text
{
	class FontFace;
}

namespace hod::inline editor
{
	/// @brief The thumbnail of a face: "Aa", or the letters of the script it is for (see GetSampleText), light on dark, as the face is drawn with its settings.
	/// The one of a font: a line for each of its faces shown (Regular, Bold), at the same size.
	/// On the CPU (TextRasterizer): the importer makes it off the main thread
	class HOD_EDITOR_API FontThumbnail
	{
	public:

		static constexpr uint32_t Size = 256;

		struct Line
		{
			const FontFace*					_face = nullptr;
			FontFaceResource::RenderMode	_renderMode = FontFaceResource::RenderMode::Smooth;
			uint32_t						_pixelSize = 0;
		};

	public:

		// rgbaPixels: Size x Size, opaque. False when the face has nothing to show
		static bool Generate(const FontFace& face, FontFaceResource::RenderMode renderMode, uint32_t pixelSize, Vector<uint8_t>& rgbaPixels);
		// The text of the first one on each, one under the other
		static bool Generate(const Vector<Line>& lines, Vector<uint8_t>& rgbaPixels);
	};
}
