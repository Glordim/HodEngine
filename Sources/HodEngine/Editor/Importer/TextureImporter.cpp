#include "HodEngine/Editor/Importer/TextureImporter.h"
#include "HodEngine/Core/Stream/FileStream.h"
#include "HodEngine/Core/Stream/MemoryStream.h"
#include "HodEngine/Core/Document/Document.h"
#include "HodEngine/Core/Document/DocumentWriterJson.h"
#include "HodEngine/Core/Output.h"

#include "HodEngine/Renderer/Resource/TextureResource.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace hod::editor
{
	DESCRIBE_REFLECTED_DERIVED_CLASS(TextureImporter, Importer)
	{
		core::Reflection::Property::Variable* generateMipmap = new core::Reflection::Property::Variable(core::Reflection::Property::Variable::Bool, offsetof(TextureImporter, _generateMipmap), "GenerateMimap");
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
	bool TextureImporter::WriteResource(core::FileStream& data, core::FileStream& meta, core::FileStream& resource)
	{
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
		uint8_t* pixels = stbi_load_from_memory(dataBuffer, dataSize, &x, &y, &componentCount, 4);
		if (pixels == nullptr)
		{
			OUTPUT_ERROR("TextureImporter : Can't load Texture data");
			return false;
		}

		// TODO use https://github.com/GPUOpen-Tools/compressonator and compress to the most adapted format related to platform's capabilities
		// For now always use png

		core::MemoryStream textureStream;
		int writeResult = stbi_write_png_to_func([](void *context, void *data, int len)
		{
			core::MemoryStream* textureStream = static_cast<core::MemoryStream*>(context);
			textureStream->Write(data, len);
		}, &textureStream, x, y, componentCount, pixels, x * 4);
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
}
