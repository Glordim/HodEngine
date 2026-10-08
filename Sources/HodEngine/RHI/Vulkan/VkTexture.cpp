#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Vulkan/VkTexture.hpp"

#include "HodEngine/RHI/Vulkan/RhiDeviceVulkan.hpp"
#include "HodEngine/RHI/Vulkan/CommandBufferVk.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <cstring>

#include <HodEngine/Math/Vector2.hpp>

#include <vk_mem_alloc.h>

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VkTexture::VkTexture()
	: Texture()
	{
		_textureImage = VK_NULL_HANDLE;
		_textureImageMemory = VK_NULL_HANDLE;

		_textureImageView = VK_NULL_HANDLE;
		_textureSampler = VK_NULL_HANDLE;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VkTexture::~VkTexture()
	{
		RhiDeviceVulkan* rhiDevice = (RhiDeviceVulkan*)RhiDevice::GetInstance();

		if (_textureSampler != VK_NULL_HANDLE)
		{
			rhiDevice->DeferDestroy(_textureSampler);
		}

		if (_textureImageView != VK_NULL_HANDLE)
		{
			rhiDevice->DeferDestroy(_textureImageView);
		}

		if (_textureImage != VK_NULL_HANDLE)
		{
			rhiDevice->DeferDestroy(_textureImage, _textureImageMemory);
		}

		if (_readbackBuffer != VK_NULL_HANDLE)
		{
			vmaUnmapMemory(rhiDevice->GetVmaAllocator(), _readbackBufferMemory);
			rhiDevice->DeferDestroy(_readbackBuffer, _readbackBufferMemory);
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VkImage VkTexture::GetTextureImage() const
	{
		return _textureImage;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VkImageView VkTexture::GetTextureImageView() const
	{
		return _textureImageView;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VkSampler VkTexture::GetTextureSampler() const
	{
		return _textureSampler;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool VkTexture::BuildDepth(uint32_t width, uint32_t height, const CreateInfo& /*createInfo*/)
	{
		bool ret = false;

		RhiDeviceVulkan* rhiDevice = (RhiDeviceVulkan*)RhiDevice::GetInstance();

		VkMemoryPropertyFlags memoryPropertyFlags =
			/*createInfo._allowReadWrite ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : */ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkImageTiling imageTiling = /*createInfo._allowReadWrite ? VK_IMAGE_TILING_LINEAR : */ VK_IMAGE_TILING_OPTIMAL;

		if (rhiDevice->CreateImage(width, height, VK_FORMAT_D32_SFLOAT_S8_UINT, imageTiling, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, memoryPropertyFlags, &_textureImage,
									&_textureImageMemory) == false)
		{
			goto exit;
		}

		/*
		if (rhiDevice->TransitionImageLayoutImmediate(_textureImage, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
														VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) == false)
		{
			goto exit;
		}
			*/

		if (rhiDevice->CreateImageView(_textureImage, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, &_textureImageView) == false)
		{
			goto exit;
		}

		ret = true;

	exit:

		if (ret == false)
		{
			if (_textureSampler != VK_NULL_HANDLE)
			{
				vkDestroySampler(rhiDevice->GetVkDevice(), _textureSampler, nullptr);
				_textureSampler = VK_NULL_HANDLE;
			}

			if (_textureImageView != VK_NULL_HANDLE)
			{
				vkDestroyImageView(rhiDevice->GetVkDevice(), _textureImageView, nullptr);
				_textureImageView = VK_NULL_HANDLE;
			}

			if (_textureImage != VK_NULL_HANDLE)
			{
				vkDestroyImage(rhiDevice->GetVkDevice(), _textureImage, nullptr);
				_textureImage = VK_NULL_HANDLE;
			}

			if (_textureImageMemory != VK_NULL_HANDLE)
			{
				// vkFreeMemory(rhiDevice->GetVkDevice(), _textureImageMemory, nullptr);
				vmaFreeMemory(rhiDevice->GetVmaAllocator(), _textureImageMemory);
				_textureImageMemory = VK_NULL_HANDLE;
			}
		}
		else
		{
			_width = width;
			_height = height;
		}

		return ret;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool VkTexture::BuildColor(uint32_t width, uint32_t height, const CreateInfo& createInfo)
	{
		RhiDeviceVulkan* rhiDevice = (RhiDeviceVulkan*)RhiDevice::GetInstance();

		VkBuffer       buffer = VK_NULL_HANDLE;
		VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
		bool           ret = false;

		// The image itself always stays optimal-tiled/device-local (fastest to render into); when
		// _allowReadWrite is requested, readback goes through a separate persistently-mapped staging
		// buffer (see below) instead of making the render target image itself host-visible.
		VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkImageTiling         imageTiling = VK_IMAGE_TILING_OPTIMAL;
		VkImageUsageFlags     imageUseFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		if (createInfo._allowReadWrite)
		{
			imageUseFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		SamplerCreateInfo samplerCreateInfo;
		samplerCreateInfo._wrapMode = createInfo._wrapMode;
		samplerCreateInfo._filterMode = createInfo._filterMode;

		if (rhiDevice->CreateImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, imageTiling, imageUseFlags, memoryPropertyFlags, &_textureImage, &_textureImageMemory) == false)
		{
			goto exit;
		}

		/*
		if (rhiDevice->TransitionImageLayoutImmediate(_textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) == false)
		{
			goto exit;
		}
		*/

		if (rhiDevice->CreateImageView(_textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &_textureImageView) == false)
		{
			goto exit;
		}

		if (rhiDevice->CreateSampler(&_textureSampler, samplerCreateInfo) == false)
		{
			goto exit;
		}

		if (createInfo._allowReadWrite)
		{
			VkBufferCreateInfo readbackBufferCreateInfo = {};
			readbackBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			readbackBufferCreateInfo.size = width * height * 4;
			readbackBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			readbackBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo readbackAllocCreateInfo = {};
			readbackAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;

			if (vmaCreateBuffer(rhiDevice->GetVmaAllocator(), &readbackBufferCreateInfo, &readbackAllocCreateInfo, &_readbackBuffer, &_readbackBufferMemory, nullptr) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Texture, unable to create readback buffer");
				goto exit;
			}

			if (vmaMapMemory(rhiDevice->GetVmaAllocator(), _readbackBufferMemory, &_readbackMappedData) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Texture, unable to map readback buffer");
				goto exit;
			}
		}

		ret = true;

	exit:
		if (buffer != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(rhiDevice->GetVkDevice(), buffer, nullptr);
		}
		if (bufferMemory != VK_NULL_HANDLE)
		{
			vkFreeMemory(rhiDevice->GetVkDevice(), bufferMemory, nullptr);
		}

		if (ret == false)
		{
			if (_readbackBuffer != VK_NULL_HANDLE)
			{
				if (_readbackMappedData != nullptr)
				{
					vmaUnmapMemory(rhiDevice->GetVmaAllocator(), _readbackBufferMemory);
					_readbackMappedData = nullptr;
				}
				vmaDestroyBuffer(rhiDevice->GetVmaAllocator(), _readbackBuffer, _readbackBufferMemory);
				_readbackBuffer = VK_NULL_HANDLE;
				_readbackBufferMemory = VK_NULL_HANDLE;
			}

			if (_textureSampler != VK_NULL_HANDLE)
			{
				vkDestroySampler(rhiDevice->GetVkDevice(), _textureSampler, nullptr);
				_textureSampler = VK_NULL_HANDLE;
			}

			if (_textureImageView != VK_NULL_HANDLE)
			{
				vkDestroyImageView(rhiDevice->GetVkDevice(), _textureImageView, nullptr);
				_textureImageView = VK_NULL_HANDLE;
			}

			if (_textureImage != VK_NULL_HANDLE)
			{
				vkDestroyImage(rhiDevice->GetVkDevice(), _textureImage, nullptr);
				_textureImage = VK_NULL_HANDLE;
			}

			if (_textureImageMemory != VK_NULL_HANDLE)
			{
				// vkFreeMemory(rhiDevice->GetVkDevice(), _textureImageMemory, nullptr);
				vmaFreeMemory(rhiDevice->GetVmaAllocator(), _textureImageMemory);
				_textureImageMemory = VK_NULL_HANDLE;
			}
		}
		else
		{
			_width = width;
			_height = height;
		}

		return ret;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool VkTexture::BuildBuffer(uint32_t width, uint32_t height, const uint8_t* pixels, const CreateInfo& createInfo)
	{
		RhiDeviceVulkan* rhiDevice = (RhiDeviceVulkan*)RhiDevice::GetInstance();

		VkBuffer       buffer = VK_NULL_HANDLE;
		VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
		VkDeviceSize   bufferSize = width * height * 4;
		void*          data = nullptr;
		bool           ret = false;

		VkMemoryPropertyFlags memoryPropertyFlags =
			createInfo._allowReadWrite ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkImageTiling imageTiling = createInfo._allowReadWrite ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;

		SamplerCreateInfo samplerCreateInfo;
		samplerCreateInfo._wrapMode = createInfo._wrapMode;
		samplerCreateInfo._filterMode = createInfo._filterMode;

		if (rhiDevice->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &buffer,
									&bufferMemory) == false)
		{
			goto exit;
		}

		if (pixels != nullptr)
		{
			if (vkMapMemory(rhiDevice->GetVkDevice(), bufferMemory, 0, bufferSize, 0, &data) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Texture, unable to map memory");
				goto exit;
			}
			memcpy(data, pixels, static_cast<size_t>(bufferSize));
			vkUnmapMemory(rhiDevice->GetVkDevice(), bufferMemory);
		}

		if (rhiDevice->CreateImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, imageTiling, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, memoryPropertyFlags,
									&_textureImage, &_textureImageMemory) == false)
		{
			goto exit;
		}

		if (rhiDevice->TransitionImageLayoutImmediate(_textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) == false)
		{
			goto exit;
		}

		if (rhiDevice->CopyBufferToImage(buffer, _textureImage, width, height) == false)
		{
			goto exit;
		}

		if (rhiDevice->TransitionImageLayoutImmediate(_textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
														VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) == false)
		{
			goto exit;
		}

		if (rhiDevice->CreateImageView(_textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &_textureImageView) == false)
		{
			goto exit;
		}

		if (rhiDevice->CreateSampler(&_textureSampler, samplerCreateInfo) == false)
		{
			goto exit;
		}

		ret = true;

	exit:
		if (buffer != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(rhiDevice->GetVkDevice(), buffer, nullptr);
		}
		if (bufferMemory != VK_NULL_HANDLE)
		{
			vkFreeMemory(rhiDevice->GetVkDevice(), bufferMemory, nullptr);
		}

		if (ret == false)
		{
			if (_textureSampler != VK_NULL_HANDLE)
			{
				vkDestroySampler(rhiDevice->GetVkDevice(), _textureSampler, nullptr);
				_textureSampler = VK_NULL_HANDLE;
			}

			if (_textureImageView != VK_NULL_HANDLE)
			{
				vkDestroyImageView(rhiDevice->GetVkDevice(), _textureImageView, nullptr);
				_textureImageView = VK_NULL_HANDLE;
			}

			if (_textureImage != VK_NULL_HANDLE)
			{
				vkDestroyImage(rhiDevice->GetVkDevice(), _textureImage, nullptr);
				_textureImage = VK_NULL_HANDLE;
			}

			if (_textureImageMemory != VK_NULL_HANDLE)
			{
				// vkFreeMemory(rhiDevice->GetVkDevice(), _textureImageMemory, nullptr);
				vmaFreeMemory(rhiDevice->GetVmaAllocator(), _textureImageMemory);
				_textureImageMemory = VK_NULL_HANDLE;
			}
		}
		else
		{
			_width = width;
			_height = height;
		}

		return ret;
	}

	/// @brief
	/// @param position
	/// @return
	Color VkTexture::ReadPixel(const Vector2& position) const
	{
		if (position.GetX() < 0 || position.GetX() > _width || position.GetY() < 0 || position.GetY() > _height)
		{
			return Color(0.0f, 0.0f, 0.0f, 0.0f);
		}

		if (_readbackReady == false)
		{
			return Color(0.0f, 0.0f, 0.0f, 0.0f);
		}

		const uint8_t* pixelData = (const uint8_t*)_readbackMappedData + ((uint32_t)position.GetY() * _width + (uint32_t)position.GetX()) * 4;

		return Color(((float)pixelData[0]) / 255.0f, ((float)pixelData[1]) / 255.0f, ((float)pixelData[2]) / 255.0f, ((float)pixelData[3]) / 255.0f);
	}

	/// @brief Records, into commandBuffer, a copy of this texture's current content into its
	/// persistently-mapped readback buffer, so a later ReadPixel() call can read it back for free.
	/// No-op if this texture wasn't built with CreateInfo::_allowReadWrite.
	/// @param commandBuffer
	void VkTexture::CaptureReadback(CommandBuffer* commandBuffer)
	{
		if (_readbackBuffer == VK_NULL_HANDLE)
		{
			return;
		}

		RhiDeviceVulkan* rhiDevice = (RhiDeviceVulkan*)RhiDevice::GetInstance();
		VkCommandBuffer vkCommandBuffer = static_cast<CommandBufferVk*>(commandBuffer)->GetVkCommandBuffer();

		rhiDevice->TransitionImageLayout(vkCommandBuffer, _textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		rhiDevice->CopyImageToBuffer(vkCommandBuffer, _textureImage, _readbackBuffer, _width, _height);
		rhiDevice->TransitionImageLayout(vkCommandBuffer, _textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		_readbackReady = true;
	}
}
