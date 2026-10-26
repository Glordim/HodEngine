#pragma once
#include "HodEngine/Text/Export.hpp"

#include <HodEngine/Core/StringView.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <cstdint>

namespace hod::inline text
{
	/// @brief A character a font draws something for: assigned, not a control, a format character, a surrogate nor a space or separator.
	/// The characters of a block a font is checked against (a glyph grid...)
	HOD_TEXT_API bool IsDrawnCharacter(char32_t codepoint);

	// utf8: at least 4 bytes. Returns the bytes written
	HOD_TEXT_API uint32_t EncodeUtf8(char32_t codepoint, char* utf8);

	// Appended to codepoints, an invalid sequence as U+FFFD (one per byte it can't read)
	HOD_TEXT_API void DecodeUtf8(StringView utf8, Vector<char32_t>& codepoints);
}
