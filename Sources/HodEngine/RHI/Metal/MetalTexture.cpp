#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Metal/MetalTexture.hpp"
#include "HodEngine/RHI/Metal/MetalRhiDevice.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>

#include <Metal/Metal.hpp>

#include <utility>

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	MetalTexture::MetalTexture() : Texture()
	{
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	MetalTexture::~MetalTexture()
	{
		Release();
	}

	/// @brief What was built, kept until the GPU is done with it: the texture can be built again
	void MetalTexture::Release()
	{
		if (_texture != nullptr)
		{
			MetalRhiDevice::GetInstance()->DeferReleaseResident(_texture);
			_texture = nullptr;
		}
		_readbackReady = false;
		_builtFromBuffer = false;
	}

	/// @brief
	/// @param format
	/// @return
	uint32_t MetalTexture::ToMetalPixelFormat(PixelFormat format)
	{
		switch (format)
		{
			case PixelFormat::R8G8B8A8_UNorm: return MTL::PixelFormatBGRA8Unorm;
			case PixelFormat::D16_UNorm: return MTL::PixelFormatDepth16Unorm;
			default: return MTL::PixelFormatInvalid;
		}
	}

	/// @brief Creates the texture from a descriptor holding its format, usage and storage mode
	/// @param textureDescriptor released by this function
	/// @return
	bool MetalTexture::Build(MTL::TextureDescriptor* textureDescriptor, uint32_t width, uint32_t height, const CreateInfo& createInfo)
	{
		Release();

		textureDescriptor->setWidth(width);
		textureDescriptor->setHeight(height);
		textureDescriptor->setTextureType(MTL::TextureType2D);
		_texture = MetalRhiDevice::GetInstance()->GetDevice()->newTexture(textureDescriptor);
		textureDescriptor->release();
		if (_texture == nullptr)
		{
			OUTPUT_ERROR("Metal: Failed to create a {}x{} texture!", width, height);
			return false;
		}
		MetalRhiDevice::GetInstance()->AddResourceToResidencySet(_texture);

		_width = width;
		_height = height;
		_mipLevelCount = (uint32_t)_texture->mipmapLevelCount();
		_readable = createInfo._allowReadWrite && _texture->storageMode() == MTL::StorageModeShared; // the depth is in GPU memory only
		++_generation;

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	bool MetalTexture::BuildDepth(uint32_t width, uint32_t height, const CreateInfo& createInfo)
	{
		MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
		textureDescriptor->setPixelFormat((MTL::PixelFormat)ToMetalPixelFormat(PixelFormat::D16_UNorm)); // like the other backends: no stencil, nothing uses it
		// Only drawn into, as on the other backends: GPU memory only, the CPU never reads it
		textureDescriptor->setUsage(MTL::TextureUsageRenderTarget);
		textureDescriptor->setStorageMode(MTL::StorageModePrivate);
		if (Build(textureDescriptor, width, height, createInfo) == false)
		{
			return false;
		}
		_format = PixelFormat::D16_UNorm;
		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool MetalTexture::BuildColor(uint32_t width, uint32_t height, const CreateInfo& createInfo)
	{
		MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
		// Same format as the presentation surfaces: a MetalGraphicsPipeline is built for one format of color attachment
		textureDescriptor->setPixelFormat((MTL::PixelFormat)ToMetalPixelFormat(PixelFormat::R8G8B8A8_UNorm));
		textureDescriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
		textureDescriptor->setStorageMode(MTL::StorageModeShared); // read back by the CPU, see ReadPixel
		if (Build(textureDescriptor, width, height, createInfo) == false)
		{
			return false;
		}
		_format = PixelFormat::R8G8B8A8_UNorm;
		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool MetalTexture::BuildBuffer(uint32_t width, uint32_t height, const uint8_t* pixels, const CreateInfo& createInfo)
	{
		// TODO Metal: R8_UNorm (MTL::PixelFormatR8Unorm), see the Vulkan/D3d12 backends
		if (createInfo._format != PixelFormat::R8G8B8A8_UNorm)
		{
			OUTPUT_ERROR("Metal: Texture, BuildBuffer only takes R8G8B8A8_UNorm pixels for now");
			return false;
		}

		MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
		textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
		textureDescriptor->setUsage(MTL::TextureUsageShaderRead);
		textureDescriptor->setStorageMode(MTL::StorageModeShared); // filled by the CPU, see replaceRegion
		textureDescriptor->setMipmapLevelCount(Clamp(createInfo._mipLevelCount, 1u, ComputeMipLevelCount(width, height)));
		if (Build(textureDescriptor, width, height, createInfo) == false)
		{
			return false;
		}

		if (pixels != nullptr)
		{
			// The levels one after the other
			const uint8_t* mipPixels = pixels;
			for (uint32_t mipLevel = 0; mipLevel < _mipLevelCount; ++mipLevel)
			{
				const uint32_t mipWidth = ComputeMipSize(width, mipLevel);
				const uint32_t mipHeight = ComputeMipSize(height, mipLevel);
				_texture->replaceRegion(MTL::Region(0, 0, 0, mipWidth, mipHeight, 1), mipLevel, mipPixels, mipWidth * 4);
				mipPixels += (size_t)mipWidth * mipHeight * 4;
			}
		}

		_format = createInfo._format;
		_builtFromBuffer = true;
		return true;
	}

	/// @brief TODO Metal: see the Vulkan/D3d12 backends. replaceRegion writes at once, while the frames in flight may still read
	/// the texture: the copy has to go through a blit of the upload commands, ahead of what is submitted next.
	/// @return
	bool MetalTexture::UploadRegion(uint32_t /*x*/, uint32_t /*y*/, uint32_t /*width*/, uint32_t /*height*/, const uint8_t* /*pixels*/, uint32_t /*mipLevel*/)
	{
		OUTPUT_ERROR("Metal: Texture::UpdateRegion is not implemented yet");
		return false;
	}

	/// @brief 
	/// @param position 
	/// @return 
	Color MetalTexture::ReadPixel(const Vector2& position) const
	{
		if (position.GetX() < 0 || position.GetX() >= _width || position.GetY() < 0 || position.GetY() >= _height)
		{
			return Color(0.0f, 0.0f, 0.0f, 0.0f);
		}

		if (_readbackReady == false)
		{
			return Color(0.0f, 0.0f, 0.0f, 0.0f);
		}

		uint8_t pixelData[4] = {0, 0, 0, 0};
		_texture->getBytes(pixelData, sizeof(pixelData), MTL::Region((uint32_t)position.GetX(), (uint32_t)position.GetY(), 0, 1, 1, 1), 0);

		if (_texture->pixelFormat() == MTL::PixelFormatBGRA8Unorm)
		{
			std::swap(pixelData[0], pixelData[2]);
		}

		return Color(((float)pixelData[0]) / 255.0f, ((float)pixelData[1]) / 255.0f, ((float)pixelData[2]) / 255.0f, ((float)pixelData[3]) / 255.0f);
	}

	/// @brief Nothing to record: the texture is in memory shared with the CPU, which ReadPixel reads once the GPU is done with the frame.
	/// No-op if this texture wasn't built with CreateInfo::_allowReadWrite.
	/// @param commandBuffer
	void MetalTexture::CaptureReadback(CommandBuffer* /*commandBuffer*/)
	{
		_readbackReady = _readable;
	}

	MTL::Texture* MetalTexture::GetNativeTexture() const
	{
		return _texture;
	}
}
