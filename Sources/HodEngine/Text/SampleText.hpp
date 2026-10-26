#pragma once
#include "HodEngine/Text/Export.hpp"

#include <HodEngine/Core/String.hpp>

namespace hod::inline text
{
	class FontFace;

	/// @brief Two characters that show what a font is for (a thumbnail, a preview): "Aa", or the first letters of the script it is made for.
	/// The script with the most characters in the font: a CJK font has Latin letters too, but thousands of ideographs.
	/// Its language (Chinese, Japanese, Korean) from its name ("Noto Sans JP"...), the code pages of its OS/2 table, or its Hangul.
	/// Else its first two characters (an icon font...). Empty when it has none
	HOD_TEXT_API String GetSampleText(const FontFace& face);
}
