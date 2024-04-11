#include "HodEngine/Renderer/Resource/TextureResource.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

namespace hod::renderer
{
	DESCRIBE_REFLECTED_CLASS(TextureResource, Resource)
	{
		ADD_PROPERTY(TextureResource, _width);
		ADD_PROPERTY(TextureResource, _height);
		ADD_PROPERTY(TextureResource, _componentCount);
		ADD_PROPERTY(TextureResource, _filterMode);
		ADD_PROPERTY(TextureResource, _wrapMode);
	}

	/// @brief 
	/// @param document 
	/// @param stream 
	/// @return 
	bool TextureResource::Initialize(const Document::Node& documentNode, std::istream& stream)
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

		std::streampos initialStreamPos = stream.tellg();
		std::streampos dataOffset = dataOffsetNode->GetUInt32();

		const Document::Node* dataSizeNode = documentNode.GetChild("DataSize");
		if (dataSizeNode == nullptr)
		{
			// TODO message
			return false;
		}

		uint32_t dataSize = dataSizeNode->GetUInt32();

		stream.seekg(initialStreamPos + dataOffset, std::ios_base::beg);

		Texture::CreateInfo createInfo;
		createInfo._wrapMode = _wrapMode;
		createInfo._filterMode = _filterMode;

		char* data = new char[dataSize];
		stream.read(data, dataSize);
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
	void TextureResource::Destroy()
	{
		delete _texture;
		_texture = nullptr;
	}

	/// @brief 
	/// @return 
	Texture* TextureResource::GetTexture() const
	{
		return _texture;
	}
}
