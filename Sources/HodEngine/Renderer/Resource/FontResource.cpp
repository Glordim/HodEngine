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
#include <limits>
#include <utility>

namespace hod::inline renderer
{
	DESCRIBE_REFLECTED_ENUM(FontResource::RenderMode, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(FontResource::RenderMode::Smooth, "Smooth");
		reflectionDescriptor.AddEnumValue(FontResource::RenderMode::PixelArt, "PixelArt");
	}

	DESCRIBE_REFLECTED_ENUM(FontResource::Weight, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(FontResource::Weight::Thin, "Thin");
		reflectionDescriptor.AddEnumValue(FontResource::Weight::ExtraLight, "ExtraLight");
		reflectionDescriptor.AddEnumValue(FontResource::Weight::Light, "Light");
		reflectionDescriptor.AddEnumValue(FontResource::Weight::Regular, "Regular");
		reflectionDescriptor.AddEnumValue(FontResource::Weight::Medium, "Medium");
		reflectionDescriptor.AddEnumValue(FontResource::Weight::SemiBold, "SemiBold");
		reflectionDescriptor.AddEnumValue(FontResource::Weight::Bold, "Bold");
		reflectionDescriptor.AddEnumValue(FontResource::Weight::ExtraBold, "ExtraBold");
		reflectionDescriptor.AddEnumValue(FontResource::Weight::Black, "Black");
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

			uint32_t fallbackCount = 0;
			if (infoDataBlock->_stream->Read(&fallbackCount, sizeof(fallbackCount)) == sizeof(fallbackCount))
			{
				_fallbacks.Clear();
				for (uint32_t fallbackIndex = 0; fallbackIndex < fallbackCount; ++fallbackIndex)
				{
					uint64_t low = 0;
					uint64_t high = 0;
					infoDataBlock->_stream->Read(&low, sizeof(low));
					infoDataBlock->_stream->Read(&high, sizeof(high));
					_fallbacks.PushBack(UID(low, high));
				}
			}

			uint32_t styleCount = 0;
			if (infoDataBlock->_stream->Read(&styleCount, sizeof(styleCount)) == sizeof(styleCount))
			{
				_styles.Clear();
				for (uint32_t styleIndex = 0; styleIndex < styleCount; ++styleIndex)
				{
					uint64_t low = 0;
					uint64_t high = 0;
					Style&   style = _styles.EmplaceBack();
					infoDataBlock->_stream->Read(&low, sizeof(low));
					infoDataBlock->_stream->Read(&high, sizeof(high));
					infoDataBlock->_stream->Read(&style._weight, sizeof(style._weight));
					infoDataBlock->_stream->Read(&style._italic, sizeof(style._italic));
					style._font = UID(low, high);
				}
			}
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

	/// @brief
	/// @return
	GlyphCache::Mode FontResource::GetGlyphMode() const
	{
		return (_renderMode == RenderMode::PixelArt && _pixelSize != 0) ? GlyphCache::Mode::Pixel : GlyphCache::Mode::Bitmap;
	}

	/// @brief
	/// @return
	const Vector<UID>& FontResource::GetFallbacks() const
	{
		return _fallbacks;
	}

	/// @brief
	/// @param fallbacks
	void FontResource::SetFallbacks(const Vector<UID>& fallbacks)
	{
		_fallbacks = fallbacks;
	}

	/// @brief
	/// @return
	const Vector<FontResource::Style>& FontResource::GetStyles() const
	{
		return _styles;
	}

	/// @brief
	/// @param styles
	void FontResource::SetStyles(const Vector<Style>& styles)
	{
		_styles = styles;
	}

	/// @brief How far a weight is from the one asked for, as CSS orders them: lower is closer.
	/// Bolder than 500 asked: the bolder ones first, then the lighter. Lighter than 400: the lighter first. 400 to 500: up to 500, then the lighter, then the bolder
	/// @param weight
	/// @param wanted
	/// @return
	static uint32_t WeightDistance(FontResource::Weight weight, FontResource::Weight wanted)
	{
		const int32_t value = (int32_t)std::to_underlying(weight);
		const int32_t target = (int32_t)std::to_underlying(wanted);
		const uint32_t distance = (uint32_t)(value > target ? value - target : target - value);
		constexpr uint32_t tier = 10000; // above any distance between two weights

		if (target > 500)
		{
			return value >= target ? distance : tier + distance;
		}
		if (target < 400)
		{
			return value <= target ? distance : tier + distance;
		}
		if (value >= target && value <= 500)
		{
			return distance;
		}
		return value < target ? tier + distance : 2 * tier + distance;
	}

	/// @brief
	/// @param weight
	/// @param italic
	/// @return
	UID FontResource::FindStyle(Weight weight, bool italic) const
	{
		// The italic ones only if one is: an upright one is drawn for an italic asked for only when the family has no italic (and the other way round)
		bool italicFound = false;
		for (const Style& style : _styles)
		{
			italicFound |= style._font.IsValid() && style._italic == italic;
		}

		UID      found;
		uint32_t foundDistance = std::numeric_limits<uint32_t>::max();
		for (const Style& style : _styles)
		{
			if (style._font.IsValid() == false || (italicFound && style._italic != italic))
			{
				continue;
			}
			const uint32_t distance = WeightDistance(style._weight, weight);
			if (distance < foundDistance) // the first of them, when they are as close
			{
				found = style._font;
				foundDistance = distance;
			}
		}
		return found;
	}
}
