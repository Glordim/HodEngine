#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/RHI/RhiDevice.hpp"
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
		AddPropertyT(reflectionDescriptor, &TextureResource::_mipLevelCount, "_mipLevelCount");
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
		infoDataBlock->_stream->Read(&_mipLevelCount, sizeof(_mipLevelCount));
		if (_mipLevelCount == 0 || _mipLevelCount > Texture::ComputeMipLevelCount(_width, _height))
		{
			OUTPUT_ERROR("TextureResource::Initialize: invalid mip level count {}", _mipLevelCount);
			return false;
		}

		const ResourceContainer::DataBlockInfo* pixelsDataBlock = resourceContainer.FindDataBlock("Pixels");
		if (pixelsDataBlock == nullptr)
		{
			return false;
		}

		// The mip levels one after the other
		const uint32_t pixelsSize = Texture::ComputeMipChainByteSize(_width, _height, _mipLevelCount);
		uint8_t*       pixels = DefaultAllocator::GetInstance().Allocate<uint8_t>(pixelsSize);
		if (pixelsDataBlock->_stream->Read(pixels, pixelsSize) != pixelsSize)
		{
			DefaultAllocator::GetInstance().Free(pixels);
			return false;
		}

		Texture::CreateInfo createInfo;
		createInfo._width = _width;
		createInfo._height = _height;
		createInfo._mipLevelCount = _mipLevelCount;

		_texture = RhiDevice::GetInstance()->CreateTexture();
		SetSamplerStates(_filterMode, _wrapMode);
		if (_texture->Build(createInfo, pixels) == false)
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

	/// @brief
	/// @param filterMode
	/// @param wrapMode
	void TextureResource::SetSamplerStates(FilterMode filterMode, WrapMode wrapMode)
	{
		_filterMode = filterMode;
		_wrapMode = wrapMode;

		if (_texture != nullptr)
		{
			Sampler::CreateInfo samplerCreateInfo;
			samplerCreateInfo._wrapMode = _wrapMode;
			samplerCreateInfo._filterMode = _filterMode;
			_texture->SetDefaultSampler(RhiDevice::GetInstance()->GetSampler(samplerCreateInfo));
		}
	}

	/// @brief
	/// @param pixels
	/// @param mipLevelCount
	/// @return
	bool TextureResource::SetPixels(const uint8_t* pixels, uint8_t mipLevelCount)
	{
		if (_texture == nullptr || mipLevelCount == 0 || mipLevelCount > Texture::ComputeMipLevelCount(_width, _height))
		{
			return false;
		}

		// The same Texture, built again: what points to it stays valid, its default sampler is kept
		Texture::CreateInfo createInfo;
		createInfo._width = _width;
		createInfo._height = _height;
		createInfo._mipLevelCount = mipLevelCount;
		if (_texture->Build(createInfo, pixels) == false)
		{
			OUTPUT_ERROR("TextureResource::SetPixels: build texture failed");
			return false;
		}

		_mipLevelCount = mipLevelCount;
		return true;
	}
}
