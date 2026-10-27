#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Resource/FontResource.hpp"

#include "HodEngine/GameSystems/Resource/ResourceContainer.hpp"

#include <limits>
#include <utility>

namespace hod::inline renderer
{
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
		(void)reflectionDescriptor;
	}

	/// @brief
	/// @param resourceContainer
	/// @return
	bool FontResource::Initialize(const ResourceContainer& resourceContainer)
	{
		// Written by the cooker from the settings of the asset
		const ResourceContainer::DataBlockInfo* infoDataBlock = resourceContainer.FindDataBlock("Info");
		if (infoDataBlock == nullptr)
		{
			return true; // a font never saved: nothing drawn with it
		}

		uint32_t styleCount = 0;
		infoDataBlock->_stream->Read(&styleCount, sizeof(styleCount));
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
			style._face = UID(low, high);
		}

		uint32_t fallbackCount = 0;
		infoDataBlock->_stream->Read(&fallbackCount, sizeof(fallbackCount));
		_fallbacks.Clear();
		for (uint32_t fallbackIndex = 0; fallbackIndex < fallbackCount; ++fallbackIndex)
		{
			uint64_t low = 0;
			uint64_t high = 0;
			infoDataBlock->_stream->Read(&low, sizeof(low));
			infoDataBlock->_stream->Read(&high, sizeof(high));
			_fallbacks.PushBack(UID(low, high));
		}
		return true;
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
		const int32_t      value = (int32_t)std::to_underlying(weight);
		const int32_t      target = (int32_t)std::to_underlying(wanted);
		const uint32_t     distance = (uint32_t)(value > target ? value - target : target - value);
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
			italicFound |= style._face.IsValid() && style._italic == italic;
		}

		UID      found;
		uint32_t foundDistance = std::numeric_limits<uint32_t>::max();
		for (const Style& style : _styles)
		{
			if (style._face.IsValid() == false || (italicFound && style._italic != italic))
			{
				continue;
			}
			const uint32_t distance = WeightDistance(style._weight, weight);
			if (distance < foundDistance) // the first of them, when they are as close
			{
				found = style._face;
				foundDistance = distance;
			}
		}
		return found;
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
}
