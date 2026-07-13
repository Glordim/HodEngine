#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"

#include "HodEngine/GameSystems/Resource/ResourceContainer.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include <cstdint>

namespace hod::inline renderer
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
	bool TextureResource::Initialize(const ResourceContainer& resourceContainer)
	{
		const ResourceContainer::DataBlockInfo* infoDataBlock = resourceContainer.FindDataBlock("Info");
		if (infoDataBlock == nullptr)
		{
			return false;
		}

		infoDataBlock->_stream->Read(&_width, sizeof(_width));
		infoDataBlock->_stream->Read(&_height, sizeof(_height));
		infoDataBlock->_stream->Read(&_filterMode, sizeof(_filterMode));
		infoDataBlock->_stream->Read(&_wrapMode, sizeof(_wrapMode));

		const ResourceContainer::DataBlockInfo* pixelsDataBlock = resourceContainer.FindDataBlock("Pixels");
		if (pixelsDataBlock == nullptr)
		{
			return false;
		}

		uint8_t* pixels = DefaultAllocator::GetInstance().Allocate<uint8_t>(_width * _height * 4);
		if (pixelsDataBlock->_stream->Read(pixels, _width * _height * 4) == false)
		{
			DefaultAllocator::GetInstance().Free(pixels);
			return false;
		}

		Texture::CreateInfo createInfo;
		createInfo._wrapMode = _wrapMode;
		createInfo._filterMode = _filterMode;

		_texture = Renderer::GetInstance()->CreateTexture();
		if (_texture->BuildBuffer(_width, _height, pixels, createInfo) == false)
		{
			OUTPUT_ERROR("TextureResource::Initialize: load texture failed");
			DefaultAllocator::GetInstance().Delete(_texture);
			DefaultAllocator::GetInstance().Free(pixels);
			_texture = nullptr;
			return false;
		}
		DefaultAllocator::GetInstance().Free(pixels);
		return true;
	}

	/// @brief
	/// @return
	Texture* TextureResource::GetTexture() const
	{
		return _texture;
	}
}
