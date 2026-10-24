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
		_readable = false;
		_readbackReady = false;
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

	/// @brief
	/// @param createInfo
	/// @param pixels
	/// @return
	bool MetalTexture::BuildResource(const CreateInfo& createInfo, const uint8_t* pixels)
	{
		Release();

		MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
		textureDescriptor->setWidth(createInfo._width);
		textureDescriptor->setHeight(createInfo._height);
		textureDescriptor->setTextureType(MTL::TextureType2D);
		textureDescriptor->setMipmapLevelCount(createInfo._mipLevelCount);

		if ((createInfo._usage & TextureUsage::CopyDst) != TextureUsage::None)
		{
			// TODO Metal: R8_UNorm (MTL::PixelFormatR8Unorm), see the Vulkan/D3d12 backends
			if (createInfo._format != PixelFormat::R8G8B8A8_UNorm)
			{
				OUTPUT_ERROR("Metal: Texture, a CopyDst texture is only R8G8B8A8_UNorm for now");
				textureDescriptor->release();
				return false;
			}
			textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
			textureDescriptor->setStorageMode(MTL::StorageModeShared); // filled by the CPU, see replaceRegion
		}
		else if (IsDepthFormat(createInfo._format))
		{
			// Like the other backends: no stencil, nothing uses it. Only drawn into: GPU memory only, the CPU never reads it
			textureDescriptor->setPixelFormat((MTL::PixelFormat)ToMetalPixelFormat(createInfo._format));
			textureDescriptor->setStorageMode(MTL::StorageModePrivate);
		}
		else
		{
			// Same format as the presentation surfaces: a MetalGraphicsPipeline is built for one format of color attachment
			textureDescriptor->setPixelFormat((MTL::PixelFormat)ToMetalPixelFormat(createInfo._format));
			textureDescriptor->setStorageMode(MTL::StorageModeShared); // read back by the CPU, see ReadPixel
		}

		MTL::TextureUsage usage = MTL::TextureUsageUnknown;
		if ((createInfo._usage & TextureUsage::Sampled) != TextureUsage::None)
		{
			usage |= MTL::TextureUsageShaderRead;
		}
		if ((createInfo._usage & TextureUsage::RenderTarget) != TextureUsage::None)
		{
			usage |= MTL::TextureUsageRenderTarget;
		}
		textureDescriptor->setUsage(usage);

		_texture = MetalRhiDevice::GetInstance()->GetDevice()->newTexture(textureDescriptor);
		textureDescriptor->release();
		if (_texture == nullptr)
		{
			OUTPUT_ERROR("Metal: Failed to create a {}x{} texture!", createInfo._width, createInfo._height);
			return false;
		}
		MetalRhiDevice::GetInstance()->AddResourceToResidencySet(_texture);

		_readable = (createInfo._usage & TextureUsage::Readback) != TextureUsage::None;

		if (pixels != nullptr)
		{
			// The levels one after the other
			const uint32_t pixelByteSize = GetPixelByteSize(createInfo._format);
			const uint8_t* mipPixels = pixels;
			for (uint32_t mipLevel = 0; mipLevel < createInfo._mipLevelCount; ++mipLevel)
			{
				const uint32_t mipWidth = ComputeMipSize(createInfo._width, mipLevel);
				const uint32_t mipHeight = ComputeMipSize(createInfo._height, mipLevel);
				_texture->replaceRegion(MTL::Region(0, 0, 0, mipWidth, mipHeight, 1), mipLevel, mipPixels, mipWidth * pixelByteSize);
				mipPixels += (size_t)mipWidth * mipHeight * pixelByteSize;
			}
		}

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
		if (position.GetX() < 0 || position.GetX() >= GetWidth() || position.GetY() < 0 || position.GetY() >= GetHeight())
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
	/// No-op if this texture is not a Readback one.
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
