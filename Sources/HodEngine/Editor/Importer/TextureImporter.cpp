#include "HodEngine/Editor/Importer/TextureImporter.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output.hpp"

#include "HodEngine/Renderer/Resource/TextureResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize2.h>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(TextureImporterSettings, ImporterSettings)
	{
		ADD_PROPERTY(TextureImporterSettings, _generateMipmap);
		ADD_PROPERTY(TextureImporterSettings, _filtering);
		ADD_PROPERTY(TextureImporterSettings, _spriteDatas);
	}

	DESCRIBE_REFLECTED_CLASS_NO_PARENT(SpriteData)
	{
		ADD_PROPERTY(SpriteData, _uid);
		ADD_PROPERTY(SpriteData, _rect);
		ADD_PROPERTY(SpriteData, _meshType);
	}

	/// @brief 
	TextureImporter::TextureImporter()
	{
		SetSupportedDataFileExtensions("png", "tga", "jpg", "bmp", "psd", "gif", "hdr", "pic");
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool TextureImporter::WriteResource(std::ifstream& data, std::ifstream& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings)
	{
		data.seekg(0, std::ios::end);
		std::streampos dataSize = data.tellg();
		data.seekg(0, std::ios::beg);
		uint8_t* dataBuffer = new uint8_t[dataSize];
		data.read(reinterpret_cast<char*>(dataBuffer), dataSize); // try ?
		if (data.fail())
		{
			OUTPUT_ERROR("TextureImporter : Can't read Texture data");
			return false;
		}

		int x;
		int y;
		int componentCount;
		uint8_t* pixels = stbi_load_from_memory(dataBuffer, dataSize, &x, &y, &componentCount, 0); // TODO rgba
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
		stbir_resize_uint8_linear(pixels, x, y, 0, thumbnailPixels, thumbnailWidth, thumbnailHeight, 0, (stbir_pixel_layout)componentCount);

		stbi_write_png_compression_level = 9;

		int writeResult = stbi_write_png_to_func([](void *context, void *data, int len)
		{
			std::ofstream* thumbnailStream= static_cast<std::ofstream*>(context);
			thumbnailStream->write(reinterpret_cast<char*>(data), len);
		}, &thumbnail, thumbnailWidth, thumbnailHeight, componentCount, thumbnailPixels, 0);
		delete[] thumbnailPixels;

		if (writeResult == 0)
		{
			OUTPUT_ERROR("TextureImporter : Can't write Thumbnail");
			stbi_image_free(pixels);
			return false;
		}

		// TODO use https://github.com/GPUOpen-Tools/compressonator and compress to the most adapted format related to platform's capabilities
		// For now always use png

		TextureImporterSettings& textureSettings = (TextureImporterSettings&)settings;

		renderer::TextureResource textureResource;
		textureResource._width = x;
		textureResource._height = y;
		textureResource._componentCount = componentCount;
		textureResource._filtering = textureSettings._filtering;

		Document document;
		if (Serializer::Serialize(textureResource, document.GetRootNode()) == false)
		{
			// TODO message
			stbi_image_free(pixels);
			return false;
		}

		document.GetRootNode().AddChild("DataOffset").SetUInt32(0);
		document.GetRootNode().AddChild("DataSize").SetUInt32(x * y * componentCount);

		std::stringstream documentStringStream;

		DocumentWriterJson documentWriter;
		if (documentWriter.Write(document, documentStringStream) == false)
		{
			// TODO message
			stbi_image_free(pixels);
			return false;
		}

		resource.write("HodResource", 11);
		uint32_t documentLen = documentStringStream.str().size();
		resource.write(reinterpret_cast<char*>(&documentLen), sizeof(documentLen));

		if (documentWriter.Write(document, resource) == false)
		{
			// TODO message
			stbi_image_free(pixels);
			return false;
		}
		resource.write(reinterpret_cast<char*>(pixels), x * y * componentCount);

		stbi_image_free(pixels);

		if (resource.fail())
		{
			// TODO message
			return false;
		}

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
		return renderer::TextureResource::GetReflectionDescriptor();
	}

	ImporterSettings* TextureImporter::AllocateSettings() const
	{
		return new TextureImporterSettings();
	}
}
