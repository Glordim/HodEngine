#include "HodEngine/Editor/Importer/TextureImporter.h"
#include "HodEngine/Core/Stream/FileStream.h"
#include "HodEngine/Core/Stream/MemoryStream.h"
#include "HodEngine/Core/Document/Document.h"
#include "HodEngine/Core/Document/DocumentWriterJson.h"
#include "HodEngine/Core/Output.h"

#include "HodEngine/Renderer/Resource/TextureResource.h"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(TextureImporterSettings, ImporterSettings)
	{
		core::Reflection::Property::Variable* generateMipmap = new core::Reflection::Property::Variable(core::Reflection::Property::Variable::Type::Bool, offsetof(TextureImporterSettings, _generateMipmap), "GenerateMimap");
		AddProperty(generateMipmap);
	}

	/// @brief 
	TextureImporter::TextureImporter()
	{
		SetSupportedDataFileExtensions("png", "tga", "jpg", "bmp", "psd", "gif", "hdr", "pic");
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool TextureImporter::WriteResource(core::FileStream& data, core::FileStream& meta, core::FileStream& resource, core::FileStream& thumbnail, ImporterSettings& settings)
	{
		TextureImporterSettings& textureSettings = (TextureImporterSettings&)settings;

		uint32_t dataSize = data.GetSize();
		uint8_t* dataBuffer = new uint8_t[dataSize];
		if (data.Read(dataBuffer, dataSize) == false)
		{
			OUTPUT_ERROR("TextureImporter : Can't read Texture data");
			return false;
		}

		int x;
		int y;
		int componentCount;
		uint8_t* pixels = stbi_load_from_memory(dataBuffer, dataSize, &x, &y, &componentCount, 3); // TODO rgba
		if (pixels == nullptr)
		{
			OUTPUT_ERROR("TextureImporter : Can't load Texture data");
			return false;
		}

		int thumbnailWidth = 256;
		int thumbnailHeight = 256;
		if (y > x)
		{
			thumbnailWidth = ((float)x / (float)y) * thumbnailHeight;
		}
		else if (x > y)
		{
			thumbnailHeight = ((float)y / (float)x) * thumbnailWidth;
		}
		uint8_t* thumbnailPixels = new uint8_t[thumbnailHeight * thumbnailWidth * componentCount];
		stbir_resize_uint8(pixels, x, y, 0, thumbnailPixels, thumbnailWidth, thumbnailHeight, 0, componentCount);

		stbi_write_png_compression_level = 9;

		int writeResult = stbi_write_png_to_func([](void *context, void *data, int len)
		{
			core::FileStream* thumbnailStream = static_cast<core::FileStream*>(context);
			thumbnailStream->Write(data, len);
		}, &thumbnail, thumbnailWidth, thumbnailHeight, componentCount, thumbnailPixels, 0);
		stbi_image_free(thumbnailPixels);

		if (writeResult == 0)
		{
			OUTPUT_ERROR("TextureImporter : Can't write Thumbnail");
			return false;
		}

		// TODO use https://github.com/GPUOpen-Tools/compressonator and compress to the most adapted format related to platform's capabilities
		// For now always use png

		core::MemoryStream textureStream;
		writeResult = stbi_write_png_to_func([](void *context, void *data, int len)
		{
			core::MemoryStream* textureStream = static_cast<core::MemoryStream*>(context);
			textureStream->Write(data, len);
		}, &textureStream, x, y, componentCount, pixels, 0);
		stbi_image_free(pixels);

		if (writeResult == 0)
		{
			OUTPUT_ERROR("TextureImporter : Can't write Texture data");
			return false;
		}

		renderer::TextureResource textureResource;
		textureResource._width = x;
		textureResource._height = y;
		textureResource._componentCount = componentCount;
		textureResource._textureInfos.resize(1);
		textureResource._textureInfos[0]._offset = 0;
		textureResource._textureInfos[0]._size = textureStream.GetSize();

		core::Document document;
		if (renderer::TextureResource::GetReflectionDescriptor()->SerializeInDocument(textureResource, document.GetRootNode()) == false)
		{
			// TODO message
			return false;
		}

		core::DocumentWriterJson documentWriter;
		if (documentWriter.Write(document, resource) == false)
		{
			// TODO message
			return false;
		}
		textureStream.Seek(0, core::Stream::SeekOrigin::Begin);
		if (resource.Write(textureStream.GetData(), textureStream.GetSize()) == false)
		{
			// TODO message
			return false;
		}

		return true;
	}

	/// @brief 
	/// @return 
	const char* TextureImporter::GetTypeName() const
	{
		return "TextureImporter";
	}

	ImporterSettings* TextureImporter::AllocateSettings() const
	{
		return new TextureImporterSettings();
	}
}
