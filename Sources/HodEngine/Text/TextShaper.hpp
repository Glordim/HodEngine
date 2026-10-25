#pragma once
#include "HodEngine/Text/Export.hpp"

#include <HodEngine/Core/ArrayView.hpp>
#include <HodEngine/Core/StringView.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <cstdint>

struct hb_buffer_t;

namespace hod::inline text
{
	class FontFace;
	struct TextLayout;

	/// @brief Turns UTF-8 text into the glyphs that draw it (kerning, ligatures, the forms of the scripts that join their letters...), with HarfBuzz.
	/// Keeps its buffers from one Shape to the next: an instance is not to be used by several threads at once.
	class HOD_TEXT_API TextShaper final
	{
	public:

						TextShaper();
						TextShaper(const TextShaper&) = delete;
						TextShaper(TextShaper&&) = delete;
						~TextShaper();

		TextShaper&		operator = (const TextShaper&) = delete;
		TextShaper&		operator = (TextShaper&&) = delete;

	public:

		// One line. faces: the first one, then the ones to fall back to, in order, for what it has no glyph for.
		// size: of the em, in the unit of the layout (pixels...). Invalid UTF-8 is shaped as U+FFFD.
		bool			Shape(StringView text, ArrayView<const FontFace*> faces, float size, TextLayout& layout);

		// Shape, when the line fits in maxWidth. Otherwise cut after the last character that fits with an ellipsis ("…", or "..." when no face has it) after it.
		// Cut between clusters (a letter keeps its marks, a ligature is kept whole), the spaces before the cut dropped. Left to right text.
		// The clusters of the glyphs before the cut are the ones of the text, the ones of the ellipsis start where the text is cut.
		bool			ShapeEllipsized(StringView text, ArrayView<const FontFace*> faces, float size, float maxWidth, TextLayout& layout);

	private:

		// In the font units of its face
		struct ShapedGlyph
		{
			uint32_t	_glyphId = 0;
			uint32_t	_faceIndex = 0;
			uint32_t	_cluster = 0;
			int32_t		_xAdvance = 0;
			int32_t		_xOffset = 0;
			int32_t		_yOffset = 0;
		};

	private:

		// Shapes [start, start + length) of the text, the rest of it as context, with faces[faceIndex].
		// What that face has no glyph for is shaped again with the next one, until the last, then with the first one, for its "missing" glyph.
		void			ShapeRange(StringView text, uint32_t start, uint32_t length, ArrayView<const FontFace*> faces, uint32_t faceIndex, bool allowFallback,
		                           Vector<ShapedGlyph>& shapedGlyphs);

	private:

		hb_buffer_t*		_hbBuffer = nullptr;
		Vector<ShapedGlyph>	_shapedGlyphs;
		Vector<char>		_ellipsizedText;
	};
}
