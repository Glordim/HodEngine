#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Stream/CompressionStream.hpp"
#include "HodEngine/Core/Stream/Stream.hpp"
#include "HodEngine/Editor/FontFaceEditor/FontFaceCooker.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <cstdint>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(FontFaceSettings, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &FontFaceSettings::_renderMode, "RenderMode");
		AddPropertyT(reflectionDescriptor, &FontFaceSettings::_pixelSize, "PixelSize");
	}

	/// @brief
	FontFaceCooker::FontFaceCooker()
	{
		SetCookerVersion(1);
	}

	bool FontFaceCooker::FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages)
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
		Vector<uint8_t>    ttf;
		ttf.Resize(ttfDataBlock->_uncompressedSize);
		if (ttfInStream.Read(ttf.Data(), ttf.Size()) != ttf.Size())
		{
			return false;
		}

		FontFaceSettings                     fontFaceSettings;
		const AssetContainer::DataBlockInfo* settingsDataBlock = assetContainer.FindDataBlock("Settings");
		if (settingsDataBlock != nullptr)
		{
			Document           settingsDocument;
			DocumentReaderJson documentReader;
			if (documentReader.Read(settingsDocument, *settingsDataBlock->_stream) == true)
			{
				Serializer::Deserialize(fontFaceSettings, settingsDocument.GetRootNode());
			}
		}

		Stream& infoOutStream = AddDataBlockStream("Info", false, std::to_underlying(Platform::All), std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
		infoOutStream.Write(&fontFaceSettings._renderMode, sizeof(fontFaceSettings._renderMode));
		infoOutStream.Write(&fontFaceSettings._pixelSize, sizeof(fontFaceSettings._pixelSize));

		Stream& ttfOutStream = AddDataBlockStream("ttf", true, std::to_underlying(Platform::All), std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
		if (ttfOutStream.Write(ttf.Data(), ttf.Size()) != ttf.Size())
		{
			return false;
		}

		return true;
	}
}
