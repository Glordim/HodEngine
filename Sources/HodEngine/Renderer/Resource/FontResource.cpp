#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/Resource/FontResource.hpp"

#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/GameSystems/Resource/ResourceContainer.hpp"

#include <HodEngine/Core/Math.hpp>

#include <cmath>
#include <cstring>

namespace hod::inline renderer
{
	DESCRIBE_REFLECTED_ENUM(FontResource::RenderMode, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(FontResource::RenderMode::Smooth, "Smooth");
		reflectionDescriptor.AddEnumValue(FontResource::RenderMode::PixelArt, "PixelArt");
	}

	DESCRIBE_REFLECTED_CLASS(FontResource, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &FontResource::_renderMode, "RenderMode", &FontResource::SetRenderMode);
		AddPropertyT(reflectionDescriptor, &FontResource::_pixelSize, "PixelSize", &FontResource::SetPixelSize);
	}

	/// @brief
	FontResource::~FontResource()
	{
		_glyphCache.Clear(); // before the face it reads
	}

	/// @brief
	/// @param document
	/// @param stream
	/// @return
	bool FontResource::Initialize(const ResourceContainer& resourceContainer)
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
			OUTPUT_ERROR("FontResource::Initialize: missing 'ttf' data block");
			return false;
		}

		uint8_t* buffer = DefaultAllocator::GetInstance().Allocate<uint8_t>(ttfDataBlock->_uncompressedSize);
		if (ttfDataBlock->_stream->Read(buffer, ttfDataBlock->_uncompressedSize) != ttfDataBlock->_uncompressedSize)
		{
			OUTPUT_ERROR("FontResource::Initialize: can't read 'ttf' data block");
			DefaultAllocator::GetInstance().Free(buffer);
			return false;
		}

		_glyphCache.Clear(); // before the face it reads is loaded again
		const bool loaded = _fontFace.LoadFromMemory(buffer, ttfDataBlock->_uncompressedSize); // copies it
		DefaultAllocator::GetInstance().Free(buffer);
		if (loaded == false)
		{
			OUTPUT_ERROR("FontResource::Initialize: load font failed");
			return false;
		}
		return _glyphCache.Init(_fontFace);
	}

	/// @brief
	/// @return
	const FontFace& FontResource::GetFontFace() const
	{
		return _fontFace;
	}

	/// @brief
	/// @return
	GlyphCache& FontResource::GetGlyphCache()
	{
		return _glyphCache;
	}

	/// @brief
	/// @return
	FontResource::RenderMode FontResource::GetRenderMode() const
	{
		return _renderMode;
	}

	/// @brief
	/// @param renderMode
	void FontResource::SetRenderMode(RenderMode renderMode)
	{
		if (_renderMode != renderMode)
		{
			_renderMode = renderMode;
			_glyphCache.Reset(); // the glyphs of the other mode are not drawn anymore
		}
	}

	/// @brief
	/// @return
	uint32_t FontResource::GetPixelSize() const
	{
		return _pixelSize;
	}

	/// @brief
	/// @param pixelSize
	void FontResource::SetPixelSize(uint32_t pixelSize)
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
	float FontResource::GetDrawnFontSize(float fontSize) const
	{
		if (_renderMode != RenderMode::PixelArt || _pixelSize == 0)
		{
			return fontSize;
		}

		const float pixelSize = (float)_pixelSize;
		return Max(1.0f, std::round(fontSize / pixelSize)) * pixelSize;
	}
}
