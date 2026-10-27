#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Resource/FontFaceResource.hpp"

#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/GameSystems/Resource/ResourceContainer.hpp"

#include <HodEngine/Core/Math.hpp>

#include <cmath>

namespace hod::inline renderer
{
	DESCRIBE_REFLECTED_ENUM(FontFaceResource::RenderMode, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(FontFaceResource::RenderMode::Smooth, "Smooth");
		reflectionDescriptor.AddEnumValue(FontFaceResource::RenderMode::PixelArt, "PixelArt");
	}

	DESCRIBE_REFLECTED_CLASS(FontFaceResource, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &FontFaceResource::_renderMode, "RenderMode", &FontFaceResource::SetRenderMode);
		AddPropertyT(reflectionDescriptor, &FontFaceResource::_pixelSize, "PixelSize", &FontFaceResource::SetPixelSize);
	}

	/// @brief
	FontFaceResource::~FontFaceResource()
	{
		_glyphCache.Clear(); // before the face it reads
	}

	/// @brief
	/// @param resourceContainer
	/// @return
	bool FontFaceResource::Initialize(const ResourceContainer& resourceContainer)
	{
		// How it is drawn: written by the cooker from the settings of the asset. Smooth without it
		const ResourceContainer::DataBlockInfo* infoDataBlock = resourceContainer.FindDataBlock("Info");
		if (infoDataBlock != nullptr)
		{
			infoDataBlock->_stream->Read(&_renderMode, sizeof(_renderMode));
			infoDataBlock->_stream->Read(&_pixelSize, sizeof(_pixelSize));
		}

		const ResourceContainer::DataBlockInfo* ttfDataBlock = resourceContainer.FindDataBlock("ttf");
		if (ttfDataBlock == nullptr)
		{
			OUTPUT_ERROR("FontFaceResource::Initialize: missing 'ttf' data block");
			return false;
		}

		uint8_t* buffer = DefaultAllocator::GetInstance().Allocate<uint8_t>(ttfDataBlock->_uncompressedSize);
		if (ttfDataBlock->_stream->Read(buffer, ttfDataBlock->_uncompressedSize) != ttfDataBlock->_uncompressedSize)
		{
			OUTPUT_ERROR("FontFaceResource::Initialize: can't read 'ttf' data block");
			DefaultAllocator::GetInstance().Free(buffer);
			return false;
		}

		_glyphCache.Clear(); // before the face it reads is loaded again
		const bool loaded = _fontFace.LoadFromMemory(buffer, ttfDataBlock->_uncompressedSize); // copies it
		DefaultAllocator::GetInstance().Free(buffer);
		if (loaded == false)
		{
			OUTPUT_ERROR("FontFaceResource::Initialize: load font failed");
			return false;
		}
		return _glyphCache.Init(_fontFace);
	}

	/// @brief
	/// @return
	const FontFace& FontFaceResource::GetFontFace() const
	{
		return _fontFace;
	}

	/// @brief
	/// @return
	GlyphCache& FontFaceResource::GetGlyphCache()
	{
		return _glyphCache;
	}

	/// @brief
	/// @return
	FontFaceResource::RenderMode FontFaceResource::GetRenderMode() const
	{
		return _renderMode;
	}

	/// @brief
	/// @param renderMode
	void FontFaceResource::SetRenderMode(RenderMode renderMode)
	{
		if (_renderMode != renderMode)
		{
			_renderMode = renderMode;
			_glyphCache.Reset(); // the glyphs of the other mode are not drawn anymore
		}
	}

	/// @brief
	/// @return
	uint32_t FontFaceResource::GetPixelSize() const
	{
		return _pixelSize;
	}

	/// @brief
	/// @param pixelSize
	void FontFaceResource::SetPixelSize(uint32_t pixelSize)
	{
		if (_pixelSize != pixelSize)
		{
			_pixelSize = pixelSize;
			if (_renderMode == RenderMode::PixelArt)
			{
				_glyphCache.Reset(); // rasterized on another grid
			}
		}
	}

	/// @brief
	/// @param fontSize
	/// @return
	float FontFaceResource::GetDrawnFontSize(float fontSize) const
	{
		if (_renderMode != RenderMode::PixelArt || _pixelSize == 0)
		{
			return fontSize;
		}

		const float pixelSize = (float)_pixelSize;
		return Max(1.0f, std::round(fontSize / pixelSize)) * pixelSize;
	}

	/// @brief
	/// @return
	GlyphCache::Mode FontFaceResource::GetGlyphMode() const
	{
		return (_renderMode == RenderMode::PixelArt && _pixelSize != 0) ? GlyphCache::Mode::Pixel : GlyphCache::Mode::Bitmap;
	}
}
