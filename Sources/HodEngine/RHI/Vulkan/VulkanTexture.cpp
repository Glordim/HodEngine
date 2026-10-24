#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Vulkan/VulkanTexture.hpp"

#include "HodEngine/RHI/Vulkan/VulkanRhiDevice.hpp"
#include "HodEngine/RHI/Vulkan/VulkanCommandBuffer.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/StaticArray.hpp>

#include <cstring>

#include <HodEngine/Math/Vector2.hpp>

#include <vk_mem_alloc.h>

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VulkanTexture::VulkanTexture()
	: Texture()
	{
		_textureImage = VK_NULL_HANDLE;
		_textureImageMemory = VK_NULL_HANDLE;

		_textureImageView = VK_NULL_HANDLE;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VulkanTexture::~VulkanTexture()
	{
		Release();
	}

	/// @brief What was built, kept until the GPU is done with it: the texture can be built again
	void VulkanTexture::Release()
	{
		VulkanRhiDevice* rhiDevice = (VulkanRhiDevice*)RhiDevice::GetInstance();

		if (_textureImageView != VK_NULL_HANDLE)
		{
			rhiDevice->DeferDestroy(_textureImageView);
			_textureImageView = VK_NULL_HANDLE;
		}

		if (_textureImage != VK_NULL_HANDLE)
		{
			rhiDevice->DeferDestroy(_textureImage, _textureImageMemory);
			_textureImage = VK_NULL_HANDLE;
			_textureImageMemory = VK_NULL_HANDLE;
		}

		if (_readbackBuffer != VK_NULL_HANDLE)
		{
			if (_readbackMappedData != nullptr)
			{
				vmaUnmapMemory(rhiDevice->GetVmaAllocator(), _readbackBufferMemory);
			}
			rhiDevice->DeferDestroy(_readbackBuffer, _readbackBufferMemory);
			_readbackBuffer = VK_NULL_HANDLE;
			_readbackBufferMemory = VK_NULL_HANDLE;
		}
		_readbackMappedData = nullptr;
		_readbackReady = false;
	}

	/// @brief
	/// @param format
	/// @return VK_FORMAT_UNDEFINED for PixelFormat::None
	VkFormat VulkanTexture::ToVkFormat(PixelFormat format)
	{
		switch (format)
		{
			case PixelFormat::R8G8B8A8_UNorm: return VK_FORMAT_R8G8B8A8_UNORM;
			case PixelFormat::D16_UNorm: return VK_FORMAT_D16_UNORM;
			case PixelFormat::R8_UNorm: return VK_FORMAT_R8_UNORM;
			default: return VK_FORMAT_UNDEFINED;
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VkImage VulkanTexture::GetTextureImage() const
	{
		return _textureImage;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VkImageView VulkanTexture::GetTextureImageView() const
	{
		return _textureImageView;
	}

	/// @brief The image always stays optimal-tiled/device-local (fastest to sample and to render into):
	/// a CopyDst one is filled through the upload commands, a Readback one is read back through a separate persistently-mapped buffer.
	/// @param createInfo
	/// @param pixels
	/// @return
	bool VulkanTexture::BuildResource(const CreateInfo& createInfo, const uint8_t* pixels)
	{
		Release(); // A texture built again: the previous image is kept until the GPU is done with it

		VulkanRhiDevice* rhiDevice = (VulkanRhiDevice*)RhiDevice::GetInstance();

		const uint32_t           width = createInfo._width;
		const uint32_t           height = createInfo._height;
		const uint32_t           mipLevelCount = createInfo._mipLevelCount;
		const VkFormat           format = ToVkFormat(createInfo._format);
		const VkImageAspectFlags aspectFlags = IsDepthFormat(createInfo._format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		const uint32_t           pixelByteSize = GetPixelByteSize(createInfo._format);

		VkImageUsageFlags imageUsageFlags = 0;
		if ((createInfo._usage & TextureUsage::Sampled) != TextureUsage::None)
		{
			imageUsageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
		}
		if ((createInfo._usage & TextureUsage::CopyDst) != TextureUsage::None)
		{
			imageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}
		if ((createInfo._usage & TextureUsage::RenderTarget) != TextureUsage::None)
		{
			imageUsageFlags |= (aspectFlags == VK_IMAGE_ASPECT_DEPTH_BIT) ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		}
		if ((createInfo._usage & TextureUsage::Readback) != TextureUsage::None)
		{
			imageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		// Filled before the image is created: a failure leaves nothing to release
		VkBuffer uploadBuffer = VK_NULL_HANDLE;
		if (pixels != nullptr)
		{
			const VkDeviceSize bufferSize = ComputeMipChainByteSize(width, height, mipLevelCount, createInfo._format);

			void* uploadData = nullptr;
			if (rhiDevice->CreateUploadBuffer(bufferSize, &uploadBuffer, &uploadData) == false)
			{
				return false;
			}
			memcpy(uploadData, pixels, static_cast<size_t>(bufferSize));
		}

		if (rhiDevice->CreateImage(width, height, format, VK_IMAGE_TILING_OPTIMAL, imageUsageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &_textureImage, &_textureImageMemory,
		                           mipLevelCount) == false)
		{
			return false;
		}

		if (rhiDevice->CreateImageView(_textureImage, format, aspectFlags, &_textureImageView, mipLevelCount) == false)
		{
			Release();
			return false;
		}

		if ((createInfo._usage & TextureUsage::Readback) != TextureUsage::None)
		{
			VkBufferCreateInfo readbackBufferCreateInfo = {};
			readbackBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			readbackBufferCreateInfo.size = (VkDeviceSize)width * height * pixelByteSize;
			readbackBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			readbackBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo readbackAllocCreateInfo = {};
			readbackAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;

			if (vmaCreateBuffer(rhiDevice->GetVmaAllocator(), &readbackBufferCreateInfo, &readbackAllocCreateInfo, &_readbackBuffer, &_readbackBufferMemory, nullptr) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Texture, unable to create readback buffer");
				Release();
				return false;
			}

			if (vmaMapMemory(rhiDevice->GetVmaAllocator(), _readbackBufferMemory, &_readbackMappedData) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Texture, unable to map readback buffer");
				_readbackMappedData = nullptr;
				Release();
				return false;
			}
		}

		if ((createInfo._usage & TextureUsage::CopyDst) != TextureUsage::None)
		{
			// Filled by the upload commands, which the CPU does not wait for: before what is submitted next draws with it.
			// The layout transitions between these layouts always succeed.
			VkCommandBuffer commandBuffer = rhiDevice->GetUploadCommandBuffer();
			if (commandBuffer == VK_NULL_HANDLE)
			{
				Release();
				return false;
			}

			rhiDevice->TransitionImageLayout(commandBuffer, _textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevelCount);

			if (uploadBuffer != VK_NULL_HANDLE)
			{
				// One region per level, one after the other in the buffer
				StaticArray<VkBufferImageCopy, 32> regions = {}; // a side of 2^32 at most
				VkDeviceSize                       bufferOffset = 0;
				for (uint32_t mipLevel = 0; mipLevel < mipLevelCount; ++mipLevel)
				{
					const uint32_t mipWidth = ComputeMipSize(width, mipLevel);
					const uint32_t mipHeight = ComputeMipSize(height, mipLevel);

					VkBufferImageCopy& region = regions[mipLevel];
					region.bufferOffset = bufferOffset;
					region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					region.imageSubresource.mipLevel = mipLevel;
					region.imageSubresource.layerCount = 1;
					region.imageExtent = {mipWidth, mipHeight, 1};

					bufferOffset += (VkDeviceSize)mipWidth * mipHeight * pixelByteSize;
				}
				vkCmdCopyBufferToImage(commandBuffer, uploadBuffer, _textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevelCount, regions.Data());
			}

			rhiDevice->TransitionImageLayout(commandBuffer, _textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevelCount);
		}

		return true;
	}

	/// @brief The region is copied by the upload commands, between two layout transitions: the draws submitted before are done with the image first
	/// @param x
	/// @param y
	/// @param width
	/// @param height
	/// @param pixels
	/// @param mipLevel
	/// @return
	bool VulkanTexture::UploadRegion(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t* pixels, uint32_t mipLevel)
	{
		VulkanRhiDevice* rhiDevice = (VulkanRhiDevice*)RhiDevice::GetInstance();

		const VkDeviceSize bufferSize = (VkDeviceSize)width * height * GetPixelByteSize(GetFormat());

		VkBuffer uploadBuffer = VK_NULL_HANDLE;
		void*    uploadData = nullptr;
		if (rhiDevice->CreateUploadBuffer(bufferSize, &uploadBuffer, &uploadData) == false)
		{
			return false;
		}
		memcpy(uploadData, pixels, static_cast<size_t>(bufferSize));

		VkCommandBuffer commandBuffer = rhiDevice->GetUploadCommandBuffer();
		if (commandBuffer == VK_NULL_HANDLE)
		{
			return false;
		}

		rhiDevice->TransitionImageLayout(commandBuffer, _textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		                                 GetMipLevelCount());

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = mipLevel;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = {(int32_t)x, (int32_t)y, 0};
		region.imageExtent = {width, height, 1};
		vkCmdCopyBufferToImage(commandBuffer, uploadBuffer, _textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		rhiDevice->TransitionImageLayout(commandBuffer, _textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		                                 GetMipLevelCount());

		return true;
	}

	/// @brief
	/// @param position
	/// @return
	Color VulkanTexture::ReadPixel(const Vector2& position) const
	{
		if (position.GetX() < 0 || position.GetX() >= GetWidth() || position.GetY() < 0 || position.GetY() >= GetHeight())
		{
			return Color(0.0f, 0.0f, 0.0f, 0.0f);
		}

		if (_readbackReady == false)
		{
			return Color(0.0f, 0.0f, 0.0f, 0.0f);
		}

		const uint8_t* pixelData = (const uint8_t*)_readbackMappedData + ((uint32_t)position.GetY() * GetWidth() + (uint32_t)position.GetX()) * 4;

		return Color(((float)pixelData[0]) / 255.0f, ((float)pixelData[1]) / 255.0f, ((float)pixelData[2]) / 255.0f, ((float)pixelData[3]) / 255.0f);
	}

	/// @brief Records, into commandBuffer, a copy of this texture's current content into its
	/// persistently-mapped readback buffer, so a later ReadPixel() call can read it back for free.
	/// No-op if this texture is not a Readback one.
	/// @param commandBuffer
	void VulkanTexture::CaptureReadback(CommandBuffer* commandBuffer)
	{
		if (_readbackBuffer == VK_NULL_HANDLE)
		{
			return;
		}

		VulkanRhiDevice* rhiDevice = (VulkanRhiDevice*)RhiDevice::GetInstance();
		VkCommandBuffer vkCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer)->GetVkCommandBuffer();

		rhiDevice->TransitionImageLayout(vkCommandBuffer, _textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		rhiDevice->CopyImageToBuffer(vkCommandBuffer, _textureImage, _readbackBuffer, GetWidth(), GetHeight());
		rhiDevice->TransitionImageLayout(vkCommandBuffer, _textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		_readbackReady = true;
	}
}
