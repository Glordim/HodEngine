#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Editor/TextureEditor/TextureImporter.hpp"

#include "HodEngine/Renderer/Resource/TextureResource.hpp"

#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <cstdint>
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize2.h>

namespace hod::inline editor
{
	/// @brief
	TextureImporter::TextureImporter()
	{
		SetSupportedDataFileExtensions("png", "tga", "jpg", "bmp", "psd", "gif", "hdr", "pic");
		SetAssetExtension("texture");
		SetAssetType("Texture");
	}

	/// @brief
	/// @param path
	/// @return
	bool TextureImporter::WriteResource(Stream& data, Stream& meta, Document& document, Vector<Resource::Data>& datas, Stream& thumbnail,
	                                    ImporterSettings& settings)
	{
		(void)data;
		(void)meta;
		(void)document;
		(void)datas;
		(void)thumbnail;
		(void)settings;
		return false;
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

		uint16_t width = x;
		uint16_t height = y;
		Stream& infoStream = AddDataBlockStream("Info", false);
		infoStream.Write(&width, sizeof(width));
		infoStream.Write(&height, sizeof(height));

		Stream& pixelsStream = AddDataBlockStream("Pixels", true);
		uint32_t pixelsSize = x * y * componentCount;
		pixelsStream.Write(pixels, pixelsSize);
		stbi_image_free(pixels);
		return true;
	}

	// TODO Move all virtual in Ctor const init ?

	/// @brief
	/// @return
	const char* TextureImporter::GetTypeName() const
	{
		return "TextureImporter";
	}

	ReflectionDescriptor* TextureImporter::GetResourceDescriptor() const
	{
		return &TextureResource::GetReflectionDescriptor();
	}

	std::shared_ptr<ImporterSettings> TextureImporter::AllocateSettings() const
	{
		return nullptr;//std::make_shared<TextureImporterSettings>();
	}
}
