#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Editor/TextureEditor/TextureImporter.hpp"

#include <cstdint>
#include <stb_image.h>

namespace hod::inline editor
{
	/// @brief
	TextureImporter::TextureImporter()
	{
		SetAssetExtension("texture");
		SetAssetType("Texture");
	}

	bool TextureImporter::FillDataBlock(Stream& source, ImporterSettings* /*importSettings*/)
	{
		uint32_t dataSize = source.GetSize();
		uint8_t* dataBuffer = DefaultAllocator::GetInstance().Allocate<uint8_t>(dataSize);
		if (source.Read(dataBuffer, dataSize) != dataSize)
		{
			OUTPUT_ERROR("TextureImporter : Can't read Texture data");
			return false;
		}

		// Always stored as RGBA8, whatever the source layout (grey, RGB, ...): the TextureCooker and the RHI expect 4 components
		constexpr uint32_t componentCount = 4;

		int      x;
		int      y;
		int      sourceComponentCount;
		uint8_t* pixels = stbi_load_from_memory(dataBuffer, (int)dataSize, &x, &y, &sourceComponentCount, (int)componentCount);
		DefaultAllocator::GetInstance().Free(dataBuffer);
		if (pixels == nullptr)
		{
			OUTPUT_ERROR("TextureImporter : Can't load Texture data");
			return false;
		}

		uint16_t width = x;
		uint16_t height = y;
		Stream& infoStream = AddDataBlockStream("Info", false);
		infoStream.Write(&width, sizeof(width));
		infoStream.Write(&height, sizeof(height));

		Stream& pixelsStream = AddDataBlockStream("Pixels", true);
		uint32_t pixelsSize = x * y * componentCount;
		pixelsStream.Write(pixels, pixelsSize);

		SetThumbnail(pixels, width, height);
		stbi_image_free(pixels);
		return true;
	}
}
