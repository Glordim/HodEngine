#include "HodEngine/Text/Pch.hpp"
#include "HodEngine/Text/TextShaper.hpp"

#include "HodEngine/Text/FontFace.hpp"
#include "HodEngine/Text/TextLayout.hpp"

#include <HodEngine/Core/Math.hpp>

#include <hb.h>

namespace hod::inline text
{
	/// @brief
	TextShaper::TextShaper()
	{
		_hbBuffer = hb_buffer_create();
	}

	/// @brief
	TextShaper::~TextShaper()
	{
		hb_buffer_destroy(_hbBuffer);
	}

	/// @brief
	/// @param text
	/// @param faces
	/// @param size
	/// @param layout
	/// @return
	bool TextShaper::Shape(StringView text, ArrayView<const FontFace*> faces, float size, TextLayout& layout)
	{
		layout.Clear();

		if (faces.Empty())
		{
			OUTPUT_ERROR("TextShaper::Shape: no face");
			return false;
		}
		for (const FontFace* face : faces)
		{
			if (face == nullptr || face->IsLoaded() == false)
			{
				OUTPUT_ERROR("TextShaper::Shape: a face is not loaded");
				return false;
			}
		}
		if (size <= 0.0f)
		{
			OUTPUT_ERROR("TextShaper::Shape: size {} is not positive", size);
			return false;
		}

		_shapedGlyphs.Clear();
		if (text.Empty() == false)
		{
			ShapeRange(text, 0, text.Size(), faces, 0, true, _shapedGlyphs);
		}

		// The metrics of the line are the ones of the faces it uses, the first one always
		Vector<bool> usedFaces;
		usedFaces.Resize(faces.Size(), false);
		usedFaces[0] = true;

		layout._glyphs.Reserve(_shapedGlyphs.Size());

		float penX = 0.0f;
		for (const ShapedGlyph& shapedGlyph : _shapedGlyphs)
		{
			const float scale = size / (float)faces[shapedGlyph._faceIndex]->GetUnitsPerEm();

			TextLayout::Glyph& glyph = layout._glyphs.EmplaceBack();
			glyph._glyphId = shapedGlyph._glyphId;
			glyph._faceIndex = shapedGlyph._faceIndex;
			glyph._cluster = shapedGlyph._cluster;
			glyph._position = Vector2(penX + (float)shapedGlyph._xOffset * scale, (float)shapedGlyph._yOffset * scale);
			glyph._advance = (float)shapedGlyph._xAdvance * scale;

			penX += glyph._advance;
			usedFaces[shapedGlyph._faceIndex] = true;
		}
		layout._width = penX;

		for (uint32_t faceIndex = 0; faceIndex < faces.Size(); ++faceIndex)
		{
			if (usedFaces[faceIndex] == false)
			{
				continue;
			}

			const FontFace* face = faces[faceIndex];
			const float     scale = size / (float)face->GetUnitsPerEm();
			layout._ascender = Max(layout._ascender, (float)face->GetAscender() * scale);
			layout._descender = Min(layout._descender, (float)face->GetDescender() * scale);
			layout._lineGap = Max(layout._lineGap, (float)face->GetLineGap() * scale);
		}

		return true;
	}

	/// @brief
	/// @param text
	/// @param start
	/// @param length
	/// @param faces
	/// @param faceIndex
	/// @param allowFallback false once back to the first face, for the "missing" glyph of what none of them has
	/// @param shapedGlyphs appended to, in the order the glyphs are drawn
	void TextShaper::ShapeRange(StringView text, uint32_t start, uint32_t length, ArrayView<const FontFace*> faces, uint32_t faceIndex, bool allowFallback,
	                            Vector<ShapedGlyph>& shapedGlyphs)
	{
		hb_buffer_clear_contents(_hbBuffer);
		hb_buffer_add_utf8(_hbBuffer, text.Data(), (int)text.Size(), start, (int)length);
		hb_buffer_guess_segment_properties(_hbBuffer);
		hb_shape(faces[faceIndex]->GetHbFont(), _hbBuffer, nullptr, 0);

		uint32_t                   glyphCount = 0;
		const hb_glyph_info_t*     glyphInfos = hb_buffer_get_glyph_infos(_hbBuffer, &glyphCount);
		const hb_glyph_position_t* glyphPositions = hb_buffer_get_glyph_positions(_hbBuffer, &glyphCount);

		// Copied out of the buffer: the fallback shapes into it again
		Vector<ShapedGlyph> rangeGlyphs;
		rangeGlyphs.Resize(glyphCount);

		// A cluster (a character with its marks, a ligature...) with a missing glyph falls back as a whole
		Vector<uint32_t> missingClusters;

		for (uint32_t glyphIndex = 0; glyphIndex < glyphCount; ++glyphIndex)
		{
			ShapedGlyph& shapedGlyph = rangeGlyphs[glyphIndex];
			shapedGlyph._glyphId = glyphInfos[glyphIndex].codepoint; // after hb_shape, the glyph id
			shapedGlyph._faceIndex = faceIndex;
			shapedGlyph._cluster = glyphInfos[glyphIndex].cluster;
			shapedGlyph._xAdvance = glyphPositions[glyphIndex].x_advance;
			shapedGlyph._xOffset = glyphPositions[glyphIndex].x_offset;
			shapedGlyph._yOffset = glyphPositions[glyphIndex].y_offset;

			if (shapedGlyph._glyphId == 0)
			{
				missingClusters.PushBack(shapedGlyph._cluster);
			}
		}

		uint32_t nextFaceIndex = faceIndex + 1;
		if (nextFaceIndex == faces.Size())
		{
			nextFaceIndex = 0; // none has it: the "missing" glyph of the first face
		}

		if (missingClusters.Empty() || allowFallback == false || nextFaceIndex == faceIndex)
		{
			for (const ShapedGlyph& shapedGlyph : rangeGlyphs)
			{
				shapedGlyphs.PushBack(shapedGlyph);
			}
			return;
		}

		auto isMissing = [&missingClusters](uint32_t cluster)
		{
			for (uint32_t missingCluster : missingClusters)
			{
				if (missingCluster == cluster)
				{
					return true;
				}
			}
			return false;
		};

		uint32_t glyphIndex = 0;
		while (glyphIndex < glyphCount)
		{
			if (isMissing(rangeGlyphs[glyphIndex]._cluster) == false)
			{
				shapedGlyphs.PushBack(rangeGlyphs[glyphIndex]);
				++glyphIndex;
				continue;
			}

			// The glyphs of consecutive missing clusters, and the text they are drawn for:
			// up to the next cluster of the range, whatever the direction the glyphs are in
			uint32_t runEnd = glyphIndex;
			uint32_t textStart = rangeGlyphs[glyphIndex]._cluster;
			uint32_t lastCluster = textStart;
			while (runEnd < glyphCount && isMissing(rangeGlyphs[runEnd]._cluster))
			{
				textStart = Min(textStart, rangeGlyphs[runEnd]._cluster);
				lastCluster = Max(lastCluster, rangeGlyphs[runEnd]._cluster);
				++runEnd;
			}

			uint32_t textEnd = start + length;
			for (const ShapedGlyph& rangeGlyph : rangeGlyphs)
			{
				if (rangeGlyph._cluster > lastCluster)
				{
					textEnd = Min(textEnd, rangeGlyph._cluster);
				}
			}

			ShapeRange(text, textStart, textEnd - textStart, faces, nextFaceIndex, nextFaceIndex != 0, shapedGlyphs);
			glyphIndex = runEnd;
		}
	}
}
