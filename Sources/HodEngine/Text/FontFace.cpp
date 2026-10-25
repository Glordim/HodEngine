#include "HodEngine/Text/Pch.hpp"
#include "HodEngine/Text/FontFace.hpp"

#include <hb-ot.h>
#include <hb.h>

#include <atomic>
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
	/// @return
	hb_font_t* FontFace::GetHbFont() const
	{
		return _hbFont;
	}
}
