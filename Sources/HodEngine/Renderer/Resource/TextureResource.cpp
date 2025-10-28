#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
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
		DefaultAllocator::GetInstance().Delete(_texture);
	}

	/// @brief
	/// @param document
	/// @param stream
	/// @return
	bool TextureResource::Initialize(const Document::Node& documentNode, const Vector<Resource::Data>& datas)
	{
		if (Serializer::Deserialize(*this, documentNode) == false)
		{
			// TODO message
			return false;
		}

		if (datas.Empty())
		{
			// TODO message
			return false;
		}

		const Resource::Data& data = datas[0];

		Texture::CreateInfo createInfo;
		createInfo._wrapMode = _wrapMode;
		createInfo._filterMode = _filterMode;

		_texture = Renderer::GetInstance()->CreateTexture();
		if (_texture->BuildBuffer(_width, _height, (unsigned char*)data._buffer, createInfo) == false) // todo BuildBuffer doesn't take void* ?
		{
			DefaultAllocator::GetInstance().Delete(_texture);
			_texture = nullptr;
			return false;
		}
		return true;
	}

	/// @brief
	/// @return
	Texture* TextureResource::GetTexture() const
	{
		return _texture;
	}
}
