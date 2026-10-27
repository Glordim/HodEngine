#include "HodEngine/Text/Pch.hpp"
#include "HodEngine/Text/FontFace.hpp"

#include <hb-ot.h>
#include <hb.h>

#include <atomic>
#include <cmath>
#include <cstdlib>
#include <cstring>

namespace hod::inline text
{
	namespace
	{
		std::atomic<uint32_t> nextFontFaceId = 1;
	}

	/// @brief
	FontFace::~FontFace()
	{
		Release();
	}

	/// @brief
	void FontFace::Release()
	{
		hb_font_destroy(_hbFont);
		_hbFont = nullptr;
		hb_face_destroy(_hbFace);
		_hbFace = nullptr;
		hb_blob_destroy(_hbBlob); // after the face that reads it
		_hbBlob = nullptr;

		_data.Clear();
		_faceIndex = 0;
		_id = 0;
		_unitsPerEm = 0;
		_ascender = 0;
		_descender = 0;
		_lineGap = 0;
		_capHeight = 0;
		_xHeight = 0;
	}

	/// @brief
	/// @param data
	/// @param size
	/// @param faceIndex
	/// @return
	bool FontFace::LoadFromMemory(const void* data, uint32_t size, uint32_t faceIndex)
	{
		Release();

		if (data == nullptr || size == 0)
		{
			OUTPUT_ERROR("FontFace::LoadFromMemory: no data");
			return false;
		}

		_data.Resize(size);
		std::memcpy(_data.Data(), data, size);
		_faceIndex = faceIndex;

		// Read where it is: _data outlives the blob, both are released together
		_hbBlob = hb_blob_create(reinterpret_cast<const char*>(_data.Data()), size, HB_MEMORY_MODE_READONLY, nullptr, nullptr);
		_hbFace = hb_face_create(_hbBlob, faceIndex);
		if (hb_face_get_glyph_count(_hbFace) == 0) // HarfBuzz gives an empty face for what it can't read
		{
			OUTPUT_ERROR("FontFace::LoadFromMemory: not a font, or no face {} in it", faceIndex);
			Release();
			return false;
		}

		_unitsPerEm = hb_face_get_upem(_hbFace);

		_hbFont = hb_font_create(_hbFace);
		hb_ot_font_set_funcs(_hbFont);
		hb_font_set_scale(_hbFont, (int)_unitsPerEm, (int)_unitsPerEm);

		hb_position_t position = 0;
		hb_ot_metrics_get_position_with_fallback(_hbFont, HB_OT_METRICS_TAG_HORIZONTAL_ASCENDER, &position);
		_ascender = position;
		hb_ot_metrics_get_position_with_fallback(_hbFont, HB_OT_METRICS_TAG_HORIZONTAL_DESCENDER, &position);
		_descender = position;
		hb_ot_metrics_get_position_with_fallback(_hbFont, HB_OT_METRICS_TAG_HORIZONTAL_LINE_GAP, &position);
		_lineGap = position;
		hb_ot_metrics_get_position_with_fallback(_hbFont, HB_OT_METRICS_TAG_CAP_HEIGHT, &position);
		_capHeight = position;
		hb_ot_metrics_get_position_with_fallback(_hbFont, HB_OT_METRICS_TAG_X_HEIGHT, &position);
		_xHeight = position;

		_id = nextFontFaceId++;
		return true;
	}

	/// @brief
	/// @return
	bool FontFace::IsLoaded() const
	{
		return _hbFont != nullptr;
	}

	/// @brief
	/// @return
	uint32_t FontFace::GetId() const
	{
		return _id;
	}

	/// @brief
	/// @return
	const uint8_t* FontFace::GetData() const
	{
		return _data.Data();
	}

	/// @brief
	/// @return
	uint32_t FontFace::GetDataSize() const
	{
		return _data.Size();
	}

	/// @brief
	/// @return
	uint32_t FontFace::GetFaceIndex() const
	{
		return _faceIndex;
	}

	/// @brief
	/// @return
	uint32_t FontFace::GetUnitsPerEm() const
	{
		return _unitsPerEm;
	}

	/// @brief
	/// @return
	int32_t FontFace::GetAscender() const
	{
		return _ascender;
	}

	/// @brief
	/// @return
	int32_t FontFace::GetDescender() const
	{
		return _descender;
	}

	/// @brief
	/// @return
	int32_t FontFace::GetLineGap() const
	{
		return _lineGap;
	}

	/// @brief
	/// @return
	int32_t FontFace::GetCapHeight() const
	{
		return _capHeight;
	}

	/// @brief
	/// @return
	int32_t FontFace::GetXHeight() const
	{
		return _xHeight;
	}

	/// @brief
	/// @return
	uint32_t FontFace::GetGlyphCount() const
	{
		return _hbFace != nullptr ? hb_face_get_glyph_count(_hbFace) : 0;
	}

	/// @brief Of the instance of a variable font
	/// @return
	uint32_t FontFace::GetWeight() const
	{
		return _hbFont != nullptr ? (uint32_t)std::lround(hb_style_get_value(_hbFont, HB_STYLE_TAG_WEIGHT)) : 0;
	}

	/// @brief
	/// @return
	bool FontFace::IsItalic() const
	{
		return _hbFont != nullptr && (hb_style_get_value(_hbFont, HB_STYLE_TAG_ITALIC) != 0.0f || hb_style_get_value(_hbFont, HB_STYLE_TAG_SLANT_ANGLE) != 0.0f);
	}

	/// @brief
	/// @return
	float FontFace::GetWidth() const
	{
		return _hbFont != nullptr ? hb_style_get_value(_hbFont, HB_STYLE_TAG_WIDTH) : 0.0f;
	}

	namespace
	{
		/// @brief The typographic name (16, 17) when the font has one: it groups more styles than the legacy one (1, 2)
		String GetName(hb_face_t* face, hb_ot_name_id_t typographicNameId, hb_ot_name_id_t nameId)
		{
			if (face == nullptr)
			{
				return String();
			}

			for (hb_ot_name_id_t id : {typographicNameId, nameId})
			{
				char         name[256] = {};
				unsigned int nameSize = sizeof(name);
				if (hb_ot_name_get_utf8(face, id, HB_LANGUAGE_INVALID, &nameSize, name) > 0)
				{
					return String(name);
				}
			}
			return String();
		}
	}

	/// @brief
	/// @return
	String FontFace::GetFamilyName() const
	{
		return GetName(_hbFace, HB_OT_NAME_ID_TYPOGRAPHIC_FAMILY, HB_OT_NAME_ID_FONT_FAMILY);
	}

	/// @brief
	/// @return
	String FontFace::GetStyleName() const
	{
		return GetName(_hbFace, HB_OT_NAME_ID_TYPOGRAPHIC_SUBFAMILY, HB_OT_NAME_ID_FONT_SUBFAMILY);
	}

	/// @brief The coarsest grid, from 4 to 64 cells an em, the bounds and advances of the glyphs are on, in font units.
	/// Nearly all of them: a pixel font may have a few glyphs off its grid (marks drawn between two pixels...), an outline font has a few on any grid
	/// @return
	uint32_t FontFace::DetectPixelGrid() const
	{
		if (_hbFont == nullptr || _unitsPerEm == 0)
		{
			return 0;
		}

		// Not 0: on every grid, it tells none apart
		Vector<int32_t> values;
		const uint32_t  glyphCount = GetGlyphCount();
		for (uint32_t glyphId = 0; glyphId < glyphCount; ++glyphId)
		{
			hb_glyph_extents_t extents = {};
			if (hb_font_get_glyph_extents(_hbFont, glyphId, &extents))
			{
				for (int32_t value : {extents.x_bearing, extents.y_bearing, extents.width, extents.height})
				{
					if (value != 0)
					{
						values.PushBack(std::abs(value));
					}
				}
			}

			const int32_t advance = std::abs((int32_t)hb_font_get_glyph_h_advance(_hbFont, glyphId));
			if (advance != 0)
			{
				values.PushBack(advance);
			}
		}
		if (values.Empty())
		{
			return 0;
		}

		constexpr float onGridRatio = 0.95f;
		for (uint32_t pixelSize = 4; pixelSize <= 64; ++pixelSize)
		{
			const float cell = (float)_unitsPerEm / (float)pixelSize;

			uint32_t onGridCount = 0;
			for (int32_t value : values)
			{
				const float cells = (float)value / cell;
				if (std::abs(cells - std::round(cells)) * cell <= 1.0f) // a font unit of rounding
				{
					++onGridCount;
				}
			}
			if ((float)onGridCount >= onGridRatio * (float)values.Size())
			{
				return pixelSize;
			}
		}
		return 0;
	}

	/// @brief
	/// @param codepoint
	/// @return
	bool FontFace::HasGlyph(char32_t codepoint) const
	{
		if (_hbFont == nullptr)
		{
			return false;
		}

		hb_codepoint_t glyph = 0;
		return hb_font_get_nominal_glyph(_hbFont, codepoint, &glyph) != 0;
	}

	/// @brief
	/// @param codepoint
	/// @return
	uint32_t FontFace::GetGlyphId(char32_t codepoint) const
	{
		hb_codepoint_t glyph = 0;
		if (_hbFont == nullptr || hb_font_get_nominal_glyph(_hbFont, codepoint, &glyph) == 0)
		{
			return 0;
		}
		return glyph;
	}

	/// @brief
	/// @param glyphId
	/// @return
	int32_t FontFace::GetGlyphAdvance(uint32_t glyphId) const
	{
		return _hbFont != nullptr ? hb_font_get_glyph_h_advance(_hbFont, glyphId) : 0;
	}

	/// @brief
	/// @param codepoints appended to
	void FontFace::GetCodepoints(Vector<char32_t>& codepoints) const
	{
		if (_hbFace == nullptr)
		{
			return;
		}

		hb_set_t* set = hb_set_create();
		hb_face_collect_unicodes(_hbFace, set);
		hb_codepoint_t codepoint = HB_SET_VALUE_INVALID;
		while (hb_set_next(set, &codepoint))
		{
			codepoints.PushBack((char32_t)codepoint);
		}
		hb_set_destroy(set);
	}

	/// @brief
	/// @return
	hb_font_t* FontFace::GetHbFont() const
	{
		return _hbFont;
	}
}
