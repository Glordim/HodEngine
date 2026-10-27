#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Stream/CompressionStream.hpp"
#include "HodEngine/Core/Stream/Stream.hpp"
#include "HodEngine/Editor/FontEditor/FontCooker.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Text/FontFace.hpp>

#include <cstdint>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(FontSettings::Style, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &FontSettings::Style::_font, "Font");
		AddPropertyT(reflectionDescriptor, &FontSettings::Style::_weight, "Weight");
		AddPropertyT(reflectionDescriptor, &FontSettings::Style::_italic, "Italic");
	}

	DESCRIBE_REFLECTED_CLASS(FontSettings, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &FontSettings::_renderMode, "RenderMode");
		AddPropertyT(reflectionDescriptor, &FontSettings::_pixelSize, "PixelSize");
		AddPropertyT(reflectionDescriptor, &FontSettings::_fallbacks, "Fallbacks");
		AddPropertyT(reflectionDescriptor, &FontSettings::_styles, "Styles");
	}

	/// @brief The closest of the weights: a font may say 350, or nothing (0, Regular then)
	/// @param face
	/// @return
	FontSettings::Style FontSettings::DetectStyle(const FontFace& face)
	{
		const uint32_t weight = face.GetWeight();

		Style style;
		style._weight = weight == 0 ? FontResource::Weight::Regular : static_cast<FontResource::Weight>(Clamp<uint32_t>((weight + 50) / 100, 1, 9) * 100);
		style._italic = face.IsItalic();
		return style;
	}

	/// @brief
	FontCooker::FontCooker()
	{
		SetCookerVersion(4); // 1: the Info data block, 2: the fallbacks in it, 3: the styles, 4: its own style out
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

		const AssetContainer::DataBlockInfo* ttfDataBlock = assetContainer.FindDataBlock("ttf");
		if (ttfDataBlock == nullptr)
		{
			return false;
		}
		CompressionStream& ttfInStream = static_cast<CompressionStream&>(*ttfDataBlock->_stream);
		Vector<uint8_t> ttf;
		ttf.Resize(ttfDataBlock->_uncompressedSize);
		if (ttfInStream.Read(ttf.Data(), ttf.Size()) != ttf.Size())
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

		Stream& infoOutStream = AddDataBlockStream("Info", false, std::to_underlying(Platform::All), std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
		infoOutStream.Write(&fontSettings._renderMode, sizeof(fontSettings._renderMode));
		infoOutStream.Write(&fontSettings._pixelSize, sizeof(fontSettings._pixelSize));

		// None set ("None" picked) are left out
		Vector<UID> fallbacks;
		for (const WeakResource<FontResource>& fallback : fontSettings._fallbacks)
		{
			if (fallback.GetUid().IsValid() && fallback.GetUid() != asset.GetUid())
			{
				fallbacks.PushBack(fallback.GetUid());
			}
		}
		const uint32_t fallbackCount = fallbacks.Size();
		infoOutStream.Write(&fallbackCount, sizeof(fallbackCount));
		for (const UID& fallback : fallbacks)
		{
			const uint64_t low = fallback.GetLow();
			const uint64_t high = fallback.GetHigh();
			infoOutStream.Write(&low, sizeof(low));
			infoOutStream.Write(&high, sizeof(high));
		}

		// None set are left out. The font itself is not: it is drawn for the styles it is set for
		Vector<const FontSettings::Style*> styles;
		for (const FontSettings::Style& style : fontSettings._styles)
		{
			if (style._font.GetUid().IsValid())
			{
				styles.PushBack(&style);
			}
		}
		const uint32_t styleCount = styles.Size();
		infoOutStream.Write(&styleCount, sizeof(styleCount));
		for (const FontSettings::Style* style : styles)
		{
			const uint64_t low = style->_font.GetUid().GetLow();
			const uint64_t high = style->_font.GetUid().GetHigh();
			infoOutStream.Write(&low, sizeof(low));
			infoOutStream.Write(&high, sizeof(high));
			infoOutStream.Write(&style->_weight, sizeof(style->_weight));
			infoOutStream.Write(&style->_italic, sizeof(style->_italic));
		}

		Stream& ttfOutStream = AddDataBlockStream("ttf", true, std::to_underlying(Platform::All), std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
		if (ttfOutStream.Write(ttf.Data(), ttf.Size()) != ttf.Size())
		{
			return false;
		}

		return true;
	}
}
