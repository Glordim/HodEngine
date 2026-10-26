#pragma once
#include "HodEngine/Text/Export.hpp"

#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <cstdint>

struct hb_blob_t;
struct hb_face_t;
struct hb_font_t;

namespace hod::inline text
{
	/// @brief A face of a font file (TTF, OTF, or one face of a TTC): its data, read once, shaped from at any size.
	/// The metrics are in font units, see GetUnitsPerEm to scale them to a size.
	class HOD_TEXT_API FontFace final
	{
	public:

						FontFace() = default;
						FontFace(const FontFace&) = delete;
						FontFace(FontFace&&) = delete;
						~FontFace();

		FontFace&		operator = (const FontFace&) = delete;
		FontFace&		operator = (FontFace&&) = delete;

	public:

		// The data is copied. Can be called again: replaces the face
		bool			LoadFromMemory(const void* data, uint32_t size, uint32_t faceIndex = 0);
		bool			IsLoaded() const;

		// Unique to each load, 0 when not loaded: what is made from a face (its layouts...) is made again when it changes
		uint32_t		GetId() const;

		// The font file, for the rasterizer to read the same face
		const uint8_t*	GetData() const;
		uint32_t		GetDataSize() const;
		uint32_t		GetFaceIndex() const;

		uint32_t		GetUnitsPerEm() const;
		int32_t			GetAscender() const;  // above the baseline, positive
		int32_t			GetDescender() const; // below the baseline, negative
		int32_t			GetLineGap() const;   // between the descender of a line and the ascender of the next one
		int32_t			GetCapHeight() const; // the top of the flat capitals (H), above the baseline
		int32_t			GetXHeight() const;   // the top of the flat lowercase letters (x), above the baseline

		uint32_t		GetGlyphCount() const;
		String			GetFamilyName() const; // "Noto Sans", empty when the font has none
		String			GetStyleName() const;  // "Regular", "Bold Italic"..., empty when the font has none

		// Pixel art: the pixels an em of the grid every glyph is drawn on (outlines and advances on whole cells of it), 0 when there is none
		uint32_t		DetectPixelGrid() const;

		bool			HasGlyph(char32_t codepoint) const;

		// Scaled in font units: the positions it gives are in font units
		hb_font_t*		GetHbFont() const;

	private:

		void			Release();

	private:

		Vector<uint8_t>	_data;
		uint32_t		_faceIndex = 0;
		uint32_t		_id = 0;

		hb_blob_t*		_hbBlob = nullptr;
		hb_face_t*		_hbFace = nullptr;
		hb_font_t*		_hbFont = nullptr;

		uint32_t		_unitsPerEm = 0;
		int32_t			_ascender = 0;
		int32_t			_descender = 0;
		int32_t			_lineGap = 0;
		int32_t			_capHeight = 0;
		int32_t			_xHeight = 0;
	};
}
