#pragma once
#include "HodEngine/Text/Export.hpp"

#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <cstdint>

namespace hod::inline text
{
	/// @brief A line of text, shaped: its glyphs, in the order they are drawn, and its metrics, at the size it is shaped at.
	/// Positions: x to the right from the start of the line, y up from the baseline.
	/// A place in the text is always a byte offset in the UTF-8 text it is shaped from.
	struct HOD_TEXT_API TextLayout
	{
		struct Glyph
		{
			uint32_t	_glyphId = 0;   // in its face, 0 for a character none of the faces has (drawn as the "missing" box of the first face)
			uint32_t	_faceIndex = 0; // among the faces given to TextShaper::Shape
			uint32_t	_cluster = 0;   // the byte offset, in the text, of the first character this glyph is drawn for
			Vector2		_position;      // of the origin of the glyph, on the baseline
			float		_advance = 0.0f;
		};

		Vector<Glyph>	_glyphs;

		float			_width = 0.0f;     // the sum of the advances
		float			_ascender = 0.0f;  // the largest of the faces used (the first one included), positive
		float			_descender = 0.0f; // the lowest of the faces used (the first one included), negative
		float			_lineGap = 0.0f;   // the largest of the faces used (the first one included)

		float			GetLineHeight() const; // from the ascender to the descender, the line gap included

		void			Clear();
	};
}
