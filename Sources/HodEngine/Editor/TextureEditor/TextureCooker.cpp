#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/TextureEditor/TextureCooker.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Stream/SpillStream.hpp"
#include "HodEngine/Editor/Cooker/Cooker.hpp"

#include "HodEngine/Renderer/Resource/TextureResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <cstdint>
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize2.h>

namespace hod::inline editor
{
	/// @brief
	TextureCooker::TextureCooker()
	{
		SetAssetType("texture");
	}

	bool TextureCooker::FillDataBlock(const Asset& asset, uint8_t platforms, uint8_t configs, uint32_t languages)
	{
		// TODO use https://github.com/GPUOpen-Tools/compressonator and compress to the most adapted format related to platform's capabilities
		// For now always use png
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
		(void)asset;
		(void)platforms;
		(void)configs;
		(void)languages;
		return false;
	}
}
