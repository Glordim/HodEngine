#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Text/GlyphCache.hpp"
#include "HodEngine/Renderer/Text/SkylinePacker.hpp"

#include "HodEngine/Renderer/FrameResources.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/RHI/RhiDevice.hpp>
#include <HodEngine/RHI/Texture.hpp>
#include <HodEngine/Text/FontFace.hpp>

#include <freetype/freetype.h>
#include <freetype/ftmodapi.h>

#include <atomic>
#include <cstring>

namespace hod::inline renderer
{
	namespace
	{
		constexpr uint32_t MaxPixelSize = 0x3FFF;

		/// @brief
		/// @param glyphId at most 0xFFFF in an OpenType font
		/// @param mode
		/// @param pixelSize Bitmap and Pixel only, at most MaxPixelSize
		/// @return
		uint64_t MakeKey(uint32_t glyphId, GlyphCache::Mode mode, uint32_t pixelSize)
		{
			return ((uint64_t)(glyphId & 0xFFFF) << 16) | ((uint64_t)mode << 14) | (uint64_t)(pixelSize & 0x3FFF);
		}

		// Unique among all the caches: a cache made at the address of another one has other generations
		std::atomic<uint32_t> nextGeneration = 1;

		/// @brief A page grown may be in a material instance of the frame being recorded (a text drawn before it grew): deleted with it
		/// @param texture
		void DeletePageTexture(Texture* texture)
		{
			Renderer* renderer = Renderer::GetInstance();
			if (renderer != nullptr)
			{
				renderer->GetCurrentFrameResources().DeleteAfter(texture);
			}
			else
			{
				DefaultAllocator::GetInstance().Delete(texture);
			}
		}
	}

	/// @brief
	GlyphCache::~GlyphCache()
	{
		Clear();
	}

	/// @brief
	/// @param face
	/// @return
	bool GlyphCache::Init(const FontFace& face)
	{
		Clear();

		if (face.IsLoaded() == false)
		{
			OUTPUT_ERROR("GlyphCache::Init: the face is not loaded");
			return false;
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

		FT_Face ftFace = nullptr;
		if (FT_New_Memory_Face(_ftLibrary, face.GetData(), (FT_Long)face.GetDataSize(), (FT_Long)face.GetFaceIndex(), &ftFace) != 0)
		{
			OUTPUT_ERROR("GlyphCache: FreeType can't read the face");
			Clear();
			return false;
		}
		_ftFace = ftFace;

		_rasterizedGlyphCount = 0;
		return true;
	}

	/// @brief
	void GlyphCache::Clear()
	{
		ReleasePages(false); // kept until the GPU is done with them, as any texture of a resource released

		if (_ftFace != nullptr)
		{
			FT_Done_Face(_ftFace);
			_ftFace = nullptr;
		}
		if (_ftLibrary != nullptr)
		{
			FT_Done_FreeType(_ftLibrary);
			_ftLibrary = nullptr;
		}
	}

	/// @brief
	void GlyphCache::Reset()
	{
		ReleasePages(true);
		_rasterizedGlyphCount = 0;
	}

	/// @brief Every glyph and page
	/// @param untilTheEndOfTheFrame a page may be in a material instance of the frame being recorded
	void GlyphCache::ReleasePages(bool untilTheEndOfTheFrame)
	{
		_glyphs.clear();

		for (Page& page : _pages)
		{
			if (untilTheEndOfTheFrame)
			{
				DeletePageTexture(page._texture);
			}
			else
			{
				DefaultAllocator::GetInstance().Delete(page._texture);
			}
			DefaultAllocator::GetInstance().Delete(page._packer);
		}
		_pages.Clear();
		_generation = nextGeneration++;
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
	/// @param pageIndex
	/// @return
	uint32_t GlyphCache::GetPageSize(uint32_t pageIndex) const
	{
		return _pages[pageIndex]._size;
	}

	/// @brief
	/// @return
	uint32_t GlyphCache::GetGeneration() const
	{
		return _generation;
	}

	/// @brief
	/// @return
	uint32_t GlyphCache::GetRasterizedGlyphCount() const
	{
		return _rasterizedGlyphCount;
	}

	/// @brief
	/// @param glyphs appended to
	void GlyphCache::GetGlyphs(Vector<const Glyph*>& glyphs) const
	{
		for (const auto& [key, glyph] : _glyphs)
		{
			glyphs.PushBack(&glyph);
		}
	}

	/// @brief
	/// @param size
	/// @param pixels size x size
	/// @return null when it can't be created
	Texture* GlyphCache::CreatePageTexture(uint32_t size, const uint8_t* pixels)
	{
		Texture* texture = RhiDevice::GetInstance()->CreateTexture();
		if (texture->Build({._width = size, ._height = size, ._format = PixelFormat::R8_UNorm}, pixels) == false)
		{
			OUTPUT_ERROR("GlyphCache: unable to create an atlas page of {}x{}", size, size);
			DefaultAllocator::GetInstance().Delete(texture);
			return nullptr;
		}
		return texture;
	}

	/// @brief A page, cleared: what a filter reads around a glyph is empty
	/// @param size
	/// @return
	bool GlyphCache::AddPage(uint32_t size)
	{
		Page page;
		page._size = size;
		page._pixels.Resize(size * size, 0);
		page._texture = CreatePageTexture(size, page._pixels.Data());
		if (page._texture == nullptr)
		{
			return false;
		}
		page._packer = DefaultAllocator::GetInstance().New<SkylinePacker>(size, size);
		_pages.PushBack(std::move(page));
		return true;
	}

	/// @brief Twice as large, its glyphs where they are: their uvs halved
	/// @param pageIndex
	/// @return
	bool GlyphCache::GrowPage(uint32_t pageIndex)
	{
		Page&          page = _pages[pageIndex];
		const uint32_t size = page._size * 2;

		Vector<uint8_t> pixels;
		pixels.Resize(size * size, 0);
		for (uint32_t row = 0; row < page._size; ++row)
		{
			std::memcpy(pixels.Data() + row * size, page._pixels.Data() + row * page._size, page._size);
		}

		Texture* texture = CreatePageTexture(size, pixels.Data());
		if (texture == nullptr)
		{
			return false;
		}

		DeletePageTexture(page._texture);
		page._texture = texture;
		page._pixels = std::move(pixels);
		page._packer->Grow(size, size);

		const float scale = (float)page._size / (float)size;
		for (auto& [key, glyph] : _glyphs)
		{
			if (glyph._empty == false && glyph._pageIndex == pageIndex)
			{
				glyph._uvMin = glyph._uvMin * scale;
				glyph._uvMax = glyph._uvMax * scale;
			}
		}
		page._size = size;

		_generation = nextGeneration++;
		return true;
	}

	/// @brief In the last page, grown when it is full, or a new one when it can't grow anymore
	/// @param width
	/// @param height
	/// @param pageIndex
	/// @param x
	/// @param y
	/// @return
	bool GlyphCache::Allocate(uint32_t width, uint32_t height, uint32_t& pageIndex, uint32_t& x, uint32_t& y)
	{
		if (width > MaxPageSize || height > MaxPageSize)
		{
			OUTPUT_ERROR("GlyphCache: a {}x{} glyph is larger than a page", width, height);
			return false;
		}

		if (_pages.Empty() && AddPage(InitialPageSize) == false)
		{
			return false;
		}

		pageIndex = _pages.Size() - 1;
		while (_pages[pageIndex]._packer->Pack(width, height, x, y) == false)
		{
			if (_pages[pageIndex]._size < MaxPageSize)
			{
				if (GrowPage(pageIndex) == false)
				{
					return false;
				}
			}
			else
			{
				if (AddPage(MaxPageSize) == false)
				{
					return false;
				}
				pageIndex = _pages.Size() - 1;
			}
		}
		return true;
	}

	/// @brief
	/// @param glyphId
	/// @param mode
	/// @param pixelSize
	/// @return
	const GlyphCache::Glyph* GlyphCache::GetGlyph(uint32_t glyphId, Mode mode, uint32_t pixelSize)
	{
		if (_ftFace == nullptr)
		{
			OUTPUT_ERROR("GlyphCache::GetGlyph: not initialized");
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

		const uint64_t key = MakeKey(glyphId, mode, pixelSize);
		auto           it = _glyphs.find(key);
		if (it != _glyphs.end())
		{
			return &it->second;
		}

		const uint32_t emPixelSize = (mode == Mode::Sdf) ? SdfEmSize : pixelSize;
		if (FT_Set_Pixel_Sizes(_ftFace, 0, emPixelSize) != 0)
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
		if (FT_Load_Glyph(_ftFace, glyphId, loadFlags) != 0)
		{
			OUTPUT_ERROR("GlyphCache::GetGlyph: FT_Load_Glyph {} failed", glyphId);
			return nullptr;
		}

		FT_GlyphSlot slot = _ftFace->glyph;
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
		Page& page = _pages[pageIndex];

		// The rows of the bitmap may be further apart, or go up: the pitch goes down one row.
		// 1 bit (Pixel): 8 pixels a byte, the leftmost in the highest bit, on is full coverage.
		// Into the page on the CPU too, for it to be copied when it grows
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
			std::memcpy(page._pixels.Data() + (y + GlyphBorder + row) * page._size + x + GlyphBorder, pixelRow, bitmap.width);
		}
		if (page._texture->UpdateRegion(x + GlyphBorder, y + GlyphBorder, bitmap.width, bitmap.rows, _pixels.Data()) == false)
		{
			return nullptr;
		}

		const float pageSize = (float)page._size;
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
