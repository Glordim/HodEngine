#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Stream/CompressionStream.hpp"
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
	DESCRIBE_REFLECTED_CLASS(FontSettings, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &FontSettings::_renderMode, "RenderMode");
		AddPropertyT(reflectionDescriptor, &FontSettings::_pixelSize, "PixelSize");
	}

	/// @brief
	FontCooker::FontCooker()
	{
		SetCookerVersion(1); // 1: the Info data block
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

		Stream& ttfOutStream = AddDataBlockStream("ttf", true, std::to_underlying(Platform::All), std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
		if (ttfOutStream.Write(ttf.Data(), ttf.Size()) != ttf.Size())
		{
			return false;
		}

		return true;
	}
}
