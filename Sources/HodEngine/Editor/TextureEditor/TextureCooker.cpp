#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/TextureEditor/TextureCooker.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Editor/Cooker/Cooker.hpp"

#include <cstdint>
#include <type_traits>
#include <utility>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(TextureSettings, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &TextureSettings::_generateMipmap, "GenerateMipmap");
		AddPropertyT(reflectionDescriptor, &TextureSettings::_filterMode, "FilterMode");
		AddPropertyT(reflectionDescriptor, &TextureSettings::_wrapMode, "WrapMode");
	}

	static constexpr std::underlying_type_t<Platform> bcPlatforms = std::to_underlying(Platform::Windows) | std::to_underlying(Platform::Linux);
	static constexpr std::underlying_type_t<Platform> astcPlatforms = std::to_underlying(Platform::MacOs) | std::to_underlying(Platform::Ios) | std::to_underlying(Platform::Android);

	/// @brief
	TextureCooker::TextureCooker()
	{
		SetAssetType("Texture");
		SetCookerVersion(1);
	}

	bool TextureCooker::FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t /*languages*/)
	{
		AssetContainer assetContainer;
		if (assetContainer.Load(asset.GetPath()) == false)
		{
			return false;
		}

		if (configs & std::to_underlying(Config::Debug))
		{
			Stream& outDebugStream = AddDataBlockStream("Debug", false, std::to_underlying(Platform::All), std::to_underlying(Config::Debug), std::to_underlying(ResourceVariant::Language::All));

			Document debugDocument;
			debugDocument.GetRootNode().AddChild("Name").SetValue(asset.GetName());
			DocumentWriterJson documentWriter;
			documentWriter.Write(debugDocument, outDebugStream);
		}

		uint16_t width = 0;
		uint16_t height = 0;
		Stream* inInfoStream = assetContainer.FindDataBlock("Info")->_stream;
		inInfoStream->Read(&width, sizeof(width));
		inInfoStream->Read(&height, sizeof(height));

		Stream& outInfoStream = AddDataBlockStream("Info", false, std::to_underlying(Platform::All), std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
		outInfoStream.Write(&width, sizeof(width));
		outInfoStream.Write(&height, sizeof(height));

		Stream* inPixelStream = assetContainer.FindDataBlock("Pixels")->_stream;
		uint32_t pixelsSize = width * height * 4;
		uint8_t* pixels = DefaultAllocator::GetInstance().Allocate<uint8_t>(pixelsSize);
		inPixelStream->Read(pixels, pixelsSize);

		if (platforms & bcPlatforms)
		{
			uint8_t* blockCompressedPixels = pixels; // TODO use bc7enc_rdo
			uint32_t blockCompressedPixelsSize = pixelsSize;
			Stream& outPixelStream = AddDataBlockStream("Pixels", true, bcPlatforms, std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
			outPixelStream.Write(blockCompressedPixels, blockCompressedPixelsSize);
		}
		if (platforms & astcPlatforms)
		{
			uint8_t* blockCompressedPixels = pixels; // TODO use astcenc
			uint32_t blockCompressedPixelsSize = pixelsSize;
			Stream& outPixelStream = AddDataBlockStream("Pixels", true, astcPlatforms, std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
			outPixelStream.Write(blockCompressedPixels, blockCompressedPixelsSize);
		}

		DefaultAllocator::GetInstance().Free(pixels);
		return true;
	}
}

// old Thumbnail generation
		/*
		int thumbnailWidth = 256;
		int thumbnailHeight = 256;
		if (y > x)
		{
			thumbnailWidth = (int)(((float)x / (float)y) * thumbnailHeight);
		}
		else if (x > y)
		{
			thumbnailHeight = (int)(((float)y / (float)x) * thumbnailWidth);
		}
		uint8_t* thumbnailPixels = DefaultAllocator::GetInstance().Allocate<uint8_t>(thumbnailHeight * thumbnailWidth * componentCount);
		stbir_resize_uint8_linear(pixels, x, y, 0, thumbnailPixels, thumbnailWidth, thumbnailHeight, 0, (stbir_pixel_layout)componentCount);

		uint32_t dataSize = source.GetSize();
		uint8_t* dataBuffer = DefaultAllocator::GetInstance().Allocate<uint8_t>(dataSize);
		if (source.Read(dataBuffer, dataSize) != dataSize)
		{
			OUTPUT_ERROR("TextureImporter : Can't read Texture data");
			return false;
		}

		int      x;
		int      y;
		int      componentCount;
		uint8_t* pixels = stbi_load_from_memory(dataBuffer, (int)dataSize, &x, &y, &componentCount, 0); // TODO rgba
		if (pixels == nullptr)
		{
			OUTPUT_ERROR("TextureImporter : Can't load Texture data");
			return false;
		}

		DefaultAllocator::GetInstance().Free(dataBuffer);

		Stream& pixelsStream = AddDataBlockStream("Pixels", true);
		uint32_t pixelsSize = x * y * componentCount;
		return pixelsStream.Write(pixels, pixelsSize) == pixelsSize;
		*/
