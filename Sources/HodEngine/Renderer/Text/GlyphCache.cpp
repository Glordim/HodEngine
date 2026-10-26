#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Text/GlyphCache.hpp"
#include "HodEngine/Renderer/Text/SkylinePacker.hpp"

#include <HodEngine/RHI/RhiDevice.hpp>
#include <HodEngine/RHI/Texture.hpp>
#include <HodEngine/Text/FontFace.hpp>

#include <freetype/freetype.h>
#include <freetype/ftmodapi.h>

#include <cstring>

namespace hod::inline renderer
{
	namespace
	{

		/// @brief
		/// @param faceId
		/// @param glyphId at most 0xFFFF in an OpenType font
		/// @param mode
		/// @param pixelSize Bitmap and Pixel only, at most MaxPixelSize
		/// @return
		uint64_t MakeKey(uint32_t faceId, uint32_t glyphId, GlyphCache::Mode mode, uint32_t pixelSize)
		{
			return ((uint64_t)faceId << 32) | ((uint64_t)(glyphId & 0xFFFF) << 16) | ((uint64_t)mode << 14) | (uint64_t)(pixelSize & 0x3FFF);
		}

		constexpr uint32_t MaxPixelSize = 0x3FFF;
	}

	/// @brief
	GlyphCache::~GlyphCache()
	{
		Clear();

		if (_ftLibrary != nullptr)
		{
			FT_Done_FreeType(_ftLibrary);
			_ftLibrary = nullptr;
		}
	}

	/// @brief
	/// @return
	bool GlyphCache::Init()
	{
		if (_ftLibrary != nullptr)
		{
			return true;
		}

		FT_Library ftLibrary = nullptr;
		if (FT_Init_FreeType(&ftLibrary) != 0)
		{
			OUTPUT_ERROR("GlyphCache: FT_Init_FreeType failed");
			return false;
		}
		_ftLibrary = ftLibrary;

		// "sdf" rasterizes outlines, "bsdf" the glyphs that are bitmaps
		FT_Int spread = SdfSpread;
		FT_Property_Set(_ftLibrary, "sdf", "spread", &spread);
		FT_Property_Set(_ftLibrary, "bsdf", "spread", &spread);

		_rasterizedGlyphCount = 0;
		return true;
	}

	/// @brief
	void GlyphCache::Clear()
	{
		_glyphs.clear();

		for (Page& page : _pages)
		{
			DefaultAllocator::GetInstance().Delete(page._texture);
			DefaultAllocator::GetInstance().Delete(page._packer);
		}
		_pages.Clear();

		for (auto& [faceId, ftFace] : _ftFaces)
		{
			FT_Done_Face(ftFace);
		}
		_ftFaces.clear();
	}

	/// @brief
	/// @param face
	void GlyphCache::Forget(const FontFace& face)
	{
		const uint32_t faceId = face.GetId();
		if (faceId == 0)
		{
			return;
		}

		for (auto it = _glyphs.begin(); it != _glyphs.end();)
		{
			if ((uint32_t)(it->first >> 32) == faceId)
			{
				it = _glyphs.erase(it);
			}
			else
			{
				++it;
			}
		}

		auto ftFaceIt = _ftFaces.find(faceId);
		if (ftFaceIt != _ftFaces.end())
		{
			FT_Done_Face(ftFaceIt->second);
			_ftFaces.erase(ftFaceIt);
		}
	}

	/// @brief
	/// @return
	uint32_t GlyphCache::GetPageCount() const
	{
		return _pages.Size();
	}

	/// @brief
	/// @param pageIndex
	/// @return
	Texture* GlyphCache::GetPage(uint32_t pageIndex) const
	{
		return _pages[pageIndex]._texture;
	}

	/// @brief
	/// @return
	uint32_t GlyphCache::GetRasterizedGlyphCount() const
	{
		return _rasterizedGlyphCount;
	}

	/// @brief Reads the data of the FontFace, where it is
	/// @param face
	/// @return
	FT_FaceRec_* GlyphCache::GetFtFace(const FontFace& face)
	{
		auto it = _ftFaces.find(face.GetId());
		if (it != _ftFaces.end())
		{
			return it->second;
		}

		FT_Face ftFace = nullptr;
		if (FT_New_Memory_Face(_ftLibrary, face.GetData(), (FT_Long)face.GetDataSize(), (FT_Long)face.GetFaceIndex(), &ftFace) != 0)
		{
			OUTPUT_ERROR("GlyphCache: FreeType can't read the face");
			return nullptr;
		}

		_ftFaces.emplace(face.GetId(), ftFace);
		return ftFace;
	}

	/// @brief A page, cleared: what a filter reads around a glyph is empty
	/// @return
	bool GlyphCache::AddPage()
	{
		Vector<uint8_t> emptyPixels;
		emptyPixels.Resize(PageSize * PageSize, 0);

		Texture* texture = RhiDevice::GetInstance()->CreateTexture();
		if (texture->Build({._width = PageSize, ._height = PageSize, ._format = PixelFormat::R8_UNorm}, emptyPixels.Data()) == false)
		{
			OUTPUT_ERROR("GlyphCache: unable to create an atlas page");
			DefaultAllocator::GetInstance().Delete(texture);
			return false;
		}

		Page page;
		page._texture = texture;
		page._packer = DefaultAllocator::GetInstance().New<SkylinePacker>(PageSize, PageSize);
		_pages.PushBack(page);
		return true;
	}

	/// @brief In the last page, or a new one when it is full
	/// @param width
	/// @param height
	/// @param pageIndex
	/// @param x
	/// @param y
	/// @return
	bool GlyphCache::Allocate(uint32_t width, uint32_t height, uint32_t& pageIndex, uint32_t& x, uint32_t& y)
	{
		if (width > PageSize || height > PageSize)
		{
			OUTPUT_ERROR("GlyphCache: a {}x{} glyph is larger than a page", width, height);
			return false;
		}

		if (_pages.Empty() == false && _pages.Back()._packer->Pack(width, height, x, y))
		{
			pageIndex = _pages.Size() - 1;
			return true;
		}

		if (AddPage() == false)
		{
			return false;
		}
		pageIndex = _pages.Size() - 1;
		return _pages.Back()._packer->Pack(width, height, x, y);
	}

	/// @brief
	/// @param face
	/// @param glyphId
	/// @param mode
	/// @param pixelSize
	/// @return
	const GlyphCache::Glyph* GlyphCache::GetGlyph(const FontFace& face, uint32_t glyphId, Mode mode, uint32_t pixelSize)
	{
		if (_ftLibrary == nullptr || face.IsLoaded() == false)
		{
			OUTPUT_ERROR("GlyphCache::GetGlyph: not initialized, or the face is not loaded");
			return nullptr;
		}

		if (mode == Mode::Sdf)
		{
			pixelSize = 0; // one SDF glyph for every size
		}
		else if (pixelSize == 0 || pixelSize > MaxPixelSize)
		{
			OUTPUT_ERROR("GlyphCache::GetGlyph: a Bitmap or Pixel glyph needs a size, in [1, {}] pixels", MaxPixelSize);
			return nullptr;
		}

		const uint64_t key = MakeKey(face.GetId(), glyphId, mode, pixelSize);
		auto           it = _glyphs.find(key);
		if (it != _glyphs.end())
		{
			return &it->second;
		}

		FT_Face ftFace = GetFtFace(face);
		if (ftFace == nullptr)
		{
			return nullptr;
		}

		const uint32_t emPixelSize = (mode == Mode::Sdf) ? SdfEmSize : pixelSize;
		if (FT_Set_Pixel_Sizes(ftFace, 0, emPixelSize) != 0)
		{
			OUTPUT_ERROR("GlyphCache::GetGlyph: FT_Set_Pixel_Sizes {} failed", emPixelSize);
			return nullptr;
		}

		// No hinting for SDF: it is scaled, nor for Pixel: its outlines are on its grid already.
		// Light hinting for Bitmap: vertical only, the advances are the ones of the shaper
		FT_Int32       loadFlags = FT_LOAD_NO_HINTING;
		FT_Render_Mode renderMode = FT_RENDER_MODE_SDF;
		if (mode == Mode::Bitmap)
		{
			loadFlags = FT_LOAD_TARGET_LIGHT;
			renderMode = FT_RENDER_MODE_NORMAL;
		}
		else if (mode == Mode::Pixel)
		{
			loadFlags = FT_LOAD_NO_HINTING | FT_LOAD_TARGET_MONO;
			renderMode = FT_RENDER_MODE_MONO;
		}
		if (FT_Load_Glyph(ftFace, glyphId, loadFlags) != 0)
		{
			OUTPUT_ERROR("GlyphCache::GetGlyph: FT_Load_Glyph {} failed", glyphId);
			return nullptr;
		}

		FT_GlyphSlot slot = ftFace->glyph;
		++_rasterizedGlyphCount;

		Glyph glyph;

		// 26.6 fixed point pixels, at emPixelSize
		const float metricsToEms = 1.0f / (64.0f * (float)emPixelSize);
		glyph._inkMin = Vector2((float)slot->metrics.horiBearingX, (float)(slot->metrics.horiBearingY - slot->metrics.height)) * metricsToEms;
		glyph._inkMax = Vector2((float)(slot->metrics.horiBearingX + slot->metrics.width), (float)slot->metrics.horiBearingY) * metricsToEms;

		const bool emptyOutline = slot->format == FT_GLYPH_FORMAT_OUTLINE && slot->outline.n_points == 0;
		if (emptyOutline == false && FT_Render_Glyph(slot, renderMode) != 0)
		{
			OUTPUT_ERROR("GlyphCache::GetGlyph: FT_Render_Glyph {} failed", glyphId);
			return nullptr;
		}

		const FT_Bitmap& bitmap = slot->bitmap;
		if (emptyOutline || bitmap.width == 0 || bitmap.rows == 0)
		{
			glyph._empty = true;
			return &_glyphs.emplace(key, glyph).first->second;
		}

		if (bitmap.pixel_mode != FT_PIXEL_MODE_GRAY && bitmap.pixel_mode != FT_PIXEL_MODE_MONO)
		{
			OUTPUT_ERROR("GlyphCache::GetGlyph: glyph {} is not rasterized as 8 bits gray nor 1 bit", glyphId);
			return nullptr;
		}

		uint32_t pageIndex = 0;
		uint32_t x = 0;
		uint32_t y = 0;
		// The glyph, its border around it: the page is cleared, the border is empty
		const uint32_t width = bitmap.width + 2 * GlyphBorder;
		const uint32_t height = bitmap.rows + 2 * GlyphBorder;
		if (Allocate(width, height, pageIndex, x, y) == false)
		{
			return nullptr;
		}

		// The rows of the bitmap may be further apart, or go up: the pitch goes down one row.
		// 1 bit (Pixel): 8 pixels a byte, the leftmost in the highest bit, on is full coverage
		_pixels.Resize(bitmap.width * bitmap.rows);
		for (uint32_t row = 0; row < bitmap.rows; ++row)
		{
			const uint8_t* bitmapRow = bitmap.buffer + (ptrdiff_t)row * bitmap.pitch;
			uint8_t*       pixelRow = _pixels.Data() + row * bitmap.width;
			if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
			{
				for (uint32_t column = 0; column < bitmap.width; ++column)
				{
					pixelRow[column] = ((bitmapRow[column / 8] >> (7 - column % 8)) & 1) ? 255 : 0;
				}
			}
			else
			{
				std::memcpy(pixelRow, bitmapRow, bitmap.width);
			}
		}
		if (_pages[pageIndex]._texture->UpdateRegion(x + GlyphBorder, y + GlyphBorder, bitmap.width, bitmap.rows, _pixels.Data()) == false)
		{
			return nullptr;
		}

		const float pageSize = (float)PageSize;
		glyph._pageIndex = pageIndex;
		glyph._uvMin = Vector2((float)x / pageSize, (float)y / pageSize);
		glyph._uvMax = Vector2((float)(x + width) / pageSize, (float)(y + height) / pageSize);

		const float emSize = (float)emPixelSize;
		const float left = (float)((int32_t)slot->bitmap_left - (int32_t)GlyphBorder) / emSize;
		const float top = (float)(slot->bitmap_top + (int32_t)GlyphBorder) / emSize;
		glyph._boundsMin = Vector2(left, top - (float)height / emSize);
		glyph._boundsMax = Vector2(left + (float)width / emSize, top);

		return &_glyphs.emplace(key, glyph).first->second;
	}
}
