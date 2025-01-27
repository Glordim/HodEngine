#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

namespace hod::renderer
{
	DESCRIBE_REFLECTED_CLASS(TextureResource, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &TextureResource::_width, "_width");
		AddPropertyT(reflectionDescriptor, &TextureResource::_height, "_height");
		AddPropertyT(reflectionDescriptor, &TextureResource::_componentCount, "_componentCount");
		AddPropertyT(reflectionDescriptor, &TextureResource::_filterMode, "_filterMode");
		AddPropertyT(reflectionDescriptor, &TextureResource::_wrapMode, "_wrapMode");
	}

	/// @brief 
	/// @return 
	TextureResource::~TextureResource()
	{
		delete _texture;
	}

	/// @brief 
	/// @param document 
	/// @param stream 
	/// @return 
	bool TextureResource::Initialize(const Document::Node& documentNode, FileSystem::Handle& fileHandle)
	{
		if (Serializer::Deserialize(*this, documentNode) == false)
		{
			// TODO message
			return false;
		}
		
		const Document::Node* dataOffsetNode = documentNode.GetChild("DataOffset");
		if (dataOffsetNode == nullptr)
		{
			// TODO message
			return false;
		}

		uint32_t initialStreamPos = FileSystem::GetInstance()->GetOffset(fileHandle);
		uint32_t dataOffset = dataOffsetNode->GetUInt32();

		const Document::Node* dataSizeNode = documentNode.GetChild("DataSize");
		if (dataSizeNode == nullptr)
		{
			// TODO message
			return false;
		}

		uint32_t dataSize = dataSizeNode->GetUInt32();

		FileSystem::GetInstance()->Seek(fileHandle, initialStreamPos + dataOffset, FileSystem::SeekMode::Begin);

		Texture::CreateInfo createInfo;
		createInfo._wrapMode = _wrapMode;
		createInfo._filterMode = _filterMode;

		char* data = new char[dataSize];
		if (FileSystem::GetInstance()->Read(fileHandle, data, dataSize) != dataSize)
		{
			delete[] data;
			return false;
		}

		_texture = Renderer::GetInstance()->CreateTexture();
		if (_texture->BuildBuffer(_width, _height, (unsigned char*)data, createInfo) == false) // todo BuildBuffer doesn't take void* ?
		{
			delete _texture;
			_texture = nullptr;

			delete[] data;
			return false;
		}
		
		delete[] data;
		return true;
	}

	/// @brief 
	/// @return 
	Texture* TextureResource::GetTexture() const
	{
		return _texture;
	}
}
