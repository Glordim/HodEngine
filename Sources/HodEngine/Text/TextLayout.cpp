#include "HodEngine/Text/Pch.hpp"
#include "HodEngine/Text/TextLayout.hpp"

namespace hod::inline text
{
	/// @brief
	/// @return
	float TextLayout::GetLineHeight() const
	{
		return _ascender - _descender + _lineGap;
	}

	/// @brief
	void TextLayout::Clear()
	{
		_glyphs.Clear();
		_width = 0.0f;
		_ascender = 0.0f;
		_descender = 0.0f;
		_lineGap = 0.0f;
	}
}
