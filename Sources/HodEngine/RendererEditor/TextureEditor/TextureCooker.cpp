#include "HodEngine/RendererEditor/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/RendererEditor/TextureEditor/TextureCooker.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Editor/Cooker/Cooker.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/RHI/Texture.hpp"

#include <cstdint>
#include <cstring>
#include <type_traits>
#include <utility>

#include <stb_image_resize2.h>

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
		SetCookerVersion(1);
	}

	/// @brief
	/// @param pixels
	/// @param width
	/// @param height
	/// @param textureSettings
	/// @param mipChain
	/// @return
	uint8_t TextureCooker::BuildMipChain(const uint8_t* pixels, uint16_t width, uint16_t height, const TextureSettings& textureSettings, Vector<uint8_t>& mipChain)
	{
		const uint32_t mipLevelCount = textureSettings._generateMipmap ? rhi::Texture::ComputeMipLevelCount(width, height) : 1;
		if (mipLevelCount == 0)
		{
			return 0;
		}

		mipChain.Resize(rhi::Texture::ComputeMipChainByteSize(width, height, mipLevelCount));
		std::memcpy(mipChain.Data(), pixels, (size_t)width * height * 4);

		// Box filter: a texel of a level is the average of the ones it covers in the previous one, without ringing (sharp edges, pixel art).
		// In linear space (sRGB), weighted by alpha: the color of transparent texels does not bleed
		const stbir_edge edge = textureSettings._wrapMode == WrapMode::Repeat ? STBIR_EDGE_WRAP : STBIR_EDGE_CLAMP;
		uint32_t         previousOffset = 0;
		for (uint32_t mipLevel = 1; mipLevel < mipLevelCount; ++mipLevel)
		{
			const uint32_t previousWidth = rhi::Texture::ComputeMipSize(width, mipLevel - 1);
			const uint32_t previousHeight = rhi::Texture::ComputeMipSize(height, mipLevel - 1);
			const uint32_t mipWidth = rhi::Texture::ComputeMipSize(width, mipLevel);
			const uint32_t mipHeight = rhi::Texture::ComputeMipSize(height, mipLevel);
			const uint32_t offset = previousOffset + previousWidth * previousHeight * 4;

			if (stbir_resize(mipChain.Data() + previousOffset, (int)previousWidth, (int)previousHeight, 0, mipChain.Data() + offset, (int)mipWidth, (int)mipHeight, 0, STBIR_RGBA,
			                 STBIR_TYPE_UINT8_SRGB, edge, STBIR_FILTER_BOX) == nullptr)
			{
				OUTPUT_ERROR("TextureCooker: Can't generate the mip level {} ({}x{})", mipLevel, mipWidth, mipHeight);
				mipChain.Clear();
				return 0;
			}

			previousOffset = offset;
		}

		return (uint8_t)mipLevelCount;
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

		TextureSettings textureSettings;
		const AssetContainer::DataBlockInfo* settingsDataBlock = assetContainer.FindDataBlock("Settings");
		if (settingsDataBlock != nullptr)
		{
			Document           settingsDocument;
			DocumentReaderJson documentReader;
			if (documentReader.Read(settingsDocument, *settingsDataBlock->_stream) == true)
			{
				Serializer::Deserialize(textureSettings, settingsDocument.GetRootNode());
			}
		}

		Stream*  inPixelStream = assetContainer.FindDataBlock("Pixels")->_stream;
		uint32_t pixelsSize = width * height * 4;
		uint8_t* pixels = DefaultAllocator::GetInstance().Allocate<uint8_t>(pixelsSize);
		if (inPixelStream->Read(pixels, pixelsSize) != pixelsSize)
		{
			OUTPUT_ERROR("TextureCooker: Can't read the pixels of {}", asset.GetName());
			DefaultAllocator::GetInstance().Free(pixels);
			return false;
		}

		Vector<uint8_t> mipChain;
		const uint8_t   mipLevelCount = BuildMipChain(pixels, width, height, textureSettings, mipChain);
		DefaultAllocator::GetInstance().Free(pixels);
		if (mipLevelCount == 0)
		{
			return false;
		}

		Stream& outInfoStream = AddDataBlockStream("Info", false, std::to_underlying(Platform::All), std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
		outInfoStream.Write(&width, sizeof(width));
		outInfoStream.Write(&height, sizeof(height));
		outInfoStream.Write(&textureSettings._filterMode, sizeof(textureSettings._filterMode));
		outInfoStream.Write(&textureSettings._wrapMode, sizeof(textureSettings._wrapMode));
		outInfoStream.Write(&mipLevelCount, sizeof(mipLevelCount));

		if (platforms & bcPlatforms)
		{
			const uint8_t* blockCompressedPixels = mipChain.Data(); // TODO use bc7enc_rdo
			uint32_t       blockCompressedPixelsSize = (uint32_t)mipChain.Size();
			Stream&        outPixelStream = AddDataBlockStream("Pixels", true, bcPlatforms, std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
			outPixelStream.Write(blockCompressedPixels, blockCompressedPixelsSize);
		}
		if (platforms & astcPlatforms)
		{
			const uint8_t* blockCompressedPixels = mipChain.Data(); // TODO use astcenc
			uint32_t       blockCompressedPixelsSize = (uint32_t)mipChain.Size();
			Stream&        outPixelStream = AddDataBlockStream("Pixels", true, astcPlatforms, std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
			outPixelStream.Write(blockCompressedPixels, blockCompressedPixelsSize);
		}

		return true;
	}
}
