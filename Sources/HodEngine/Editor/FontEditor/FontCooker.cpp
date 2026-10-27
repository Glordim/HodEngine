#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Stream/Stream.hpp"
#include "HodEngine/Editor/FontEditor/FontCooker.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <cstdint>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(FontSettings::Style, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &FontSettings::Style::_face, "Face");
		AddPropertyT(reflectionDescriptor, &FontSettings::Style::_weight, "Weight");
		AddPropertyT(reflectionDescriptor, &FontSettings::Style::_italic, "Italic");
	}

	DESCRIBE_REFLECTED_CLASS(FontSettings, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &FontSettings::_styles, "Styles");
		AddPropertyT(reflectionDescriptor, &FontSettings::_fallbacks, "Fallbacks");
	}

	/// @brief None set ("None" picked) are left out
	/// @return
	Vector<FontResource::Style> FontSettings::GetStyles() const
	{
		Vector<FontResource::Style> styles;
		for (const Style& style : _styles)
		{
			if (style._face.GetUid().IsValid())
			{
				FontResource::Style& fontStyle = styles.EmplaceBack();
				fontStyle._face = style._face.GetUid();
				fontStyle._weight = style._weight;
				fontStyle._italic = style._italic;
			}
		}
		return styles;
	}

	/// @brief None set are left out
	/// @param font
	/// @return
	Vector<UID> FontSettings::GetFallbacks(const UID& font) const
	{
		Vector<UID> fallbacks;
		for (const WeakResource<FontResource>& fallback : _fallbacks)
		{
			if (fallback.GetUid().IsValid() == false || fallback.GetUid() == font)
			{
				continue;
			}
			bool already = false;
			for (const UID& previous : fallbacks)
			{
				already |= previous == fallback.GetUid();
			}
			if (already == false)
			{
				fallbacks.PushBack(fallback.GetUid());
			}
		}
		return fallbacks;
	}

	/// @brief
	FontCooker::FontCooker()
	{
		SetCookerVersion(1);
	}

	bool FontCooker::FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages)
	{
		(void)platforms;
		(void)configs;
		(void)languages;

		AssetContainer assetContainer;
		if (assetContainer.Load(asset.GetPath()) == false)
		{
			return false;
		}

		FontSettings                         fontSettings;
		const AssetContainer::DataBlockInfo* settingsDataBlock = assetContainer.FindDataBlock("Settings");
		if (settingsDataBlock != nullptr)
		{
			Document           settingsDocument;
			DocumentReaderJson documentReader;
			if (documentReader.Read(settingsDocument, *settingsDataBlock->_stream) == true)
			{
				Serializer::Deserialize(fontSettings, settingsDocument.GetRootNode());
			}
		}

		// As FontResource::Initialize reads it
		Stream& infoOutStream = AddDataBlockStream("Info", false, std::to_underlying(Platform::All), std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));

		const Vector<FontResource::Style> styles = fontSettings.GetStyles();
		const uint32_t                    styleCount = styles.Size();
		infoOutStream.Write(&styleCount, sizeof(styleCount));
		for (const FontResource::Style& style : styles)
		{
			const uint64_t low = style._face.GetLow();
			const uint64_t high = style._face.GetHigh();
			infoOutStream.Write(&low, sizeof(low));
			infoOutStream.Write(&high, sizeof(high));
			infoOutStream.Write(&style._weight, sizeof(style._weight));
			infoOutStream.Write(&style._italic, sizeof(style._italic));
		}

		const Vector<UID> fallbacks = fontSettings.GetFallbacks(asset.GetUid());
		const uint32_t    fallbackCount = fallbacks.Size();
		infoOutStream.Write(&fallbackCount, sizeof(fallbackCount));
		for (const UID& fallback : fallbacks)
		{
			const uint64_t low = fallback.GetLow();
			const uint64_t high = fallback.GetHigh();
			infoOutStream.Write(&low, sizeof(low));
			infoOutStream.Write(&high, sizeof(high));
		}

		return true;
	}
}
