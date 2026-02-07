#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Editor/Importer/TextureImporter.hpp"

#include "HodEngine/Renderer/Resource/TextureResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <Stb/stb_image.h>
#include <Stb/stb_image_write.h>
#include <Stb/stb_image_resize2.h>

#include <sstream>

namespace hod::editor
{
	DESCRIBE_REFLECTED_ENUM(MeshType, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(MeshType::Rect, "Rect");
		reflectionDescriptor.AddEnumValue(MeshType::Tight, "Tight");
	}

	DESCRIBE_REFLECTED_CLASS(TextureImporterSettings, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &TextureImporterSettings::_generateMipmap, "GenerateMipmap");
		AddPropertyT(reflectionDescriptor, &TextureImporterSettings::_filterMode, "FilterMode");
		AddPropertyT(reflectionDescriptor, &TextureImporterSettings::_wrapMode, "WrapMode");
		AddPropertyT(reflectionDescriptor, &TextureImporterSettings::_spriteDatas, "SpriteDatas");
	}

	DESCRIBE_REFLECTED_CLASS(SpriteData, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &SpriteData::_uid, "Uid");
		AddPropertyT(reflectionDescriptor, &SpriteData::_rect, "Rect");
		AddPropertyT(reflectionDescriptor, &SpriteData::_meshType, "MeshType");
	}

	/// @brief
	TextureImporter::TextureImporter()
	{
		SetSupportedDataFileExtensions("png", "tga", "jpg", "bmp", "psd", "gif", "hdr", "pic");
	}

	/// @brief
	/// @param path
	/// @return
	bool TextureImporter::WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, Document& document, Vector<Resource::Data>& datas, std::ofstream& thumbnail,
	                                    ImporterSettings& settings)
	{
		// TODO
		(void)meta;

		uint32_t dataSize = FileSystem::GetInstance()->GetSize(data);
		uint8_t* dataBuffer = DefaultAllocator::GetInstance().Allocate<uint8_t>(dataSize);
		if (FileSystem::GetInstance()->Read(data, reinterpret_cast<char*>(dataBuffer), dataSize) != (int32_t)dataSize)
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

		stbi_write_png_compression_level = 9;

		int writeResult = stbi_write_png_to_func(
			[](void* context, void* data, int len)
			{
				std::ofstream* thumbnailStream = static_cast<std::ofstream*>(context);
				thumbnailStream->write(reinterpret_cast<char*>(data), len);
			},
			&thumbnail, thumbnailWidth, thumbnailHeight, componentCount, thumbnailPixels, 0);
		DefaultAllocator::GetInstance().Free(thumbnailPixels);

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
		textureResource._filterMode = textureSettings._filterMode;
		textureResource._wrapMode = textureSettings._wrapMode;

		if (Serializer::Serialize(textureResource, document.GetRootNode()) == false)
		{
			// TODO message
			stbi_image_free(pixels);
			return false;
		}

		Resource::Data pixelsData;
		pixelsData._buffer = DefaultAllocator::GetInstance().Allocate(x * y * componentCount);
		std::memcpy(pixelsData._buffer, pixels, x * y * componentCount);
		pixelsData._size = x * y * componentCount;
		datas.push_back(pixelsData);

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
		return &renderer::TextureResource::GetReflectionDescriptor();
	}

	std::shared_ptr<ImporterSettings> TextureImporter::AllocateSettings() const
	{
		return std::make_shared<TextureImporterSettings>();
	}
}
