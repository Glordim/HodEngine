#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkTexture.hpp"

#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <cstring>

#include <HodEngine/Core/Math/Vector2.hpp>

#include <VulkanMemoryAllocator/vk_mem_alloc.h>

namespace hod
{
	namespace renderer
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
			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			if (_textureSampler != VK_NULL_HANDLE)
			{
				vkDestroySampler(renderer->GetVkDevice(), _textureSampler, nullptr);
			}

			if (_textureImageView != VK_NULL_HANDLE)
			{
				vkDestroyImageView(renderer->GetVkDevice(), _textureImageView, nullptr);
			}

			if (_textureImage != VK_NULL_HANDLE)
			{
				vkDestroyImage(renderer->GetVkDevice(), _textureImage, nullptr);
			}

			if (_textureImageMemory != VK_NULL_HANDLE)
			{
				vkFreeMemory(renderer->GetVkDevice(), _textureImageMemory, nullptr);
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

			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			VkMemoryPropertyFlags memoryPropertyFlags =
				/*createInfo._allowReadWrite ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : */ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			VkImageTiling imageTiling = /*createInfo._allowReadWrite ? VK_IMAGE_TILING_LINEAR : */ VK_IMAGE_TILING_OPTIMAL;

			if (renderer->CreateImage(width, height, VK_FORMAT_D32_SFLOAT_S8_UINT, imageTiling, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, memoryPropertyFlags, &_textureImage,
			                          &_textureImageMemory) == false)
			{
				goto exit;
			}

			if (renderer->TransitionImageLayoutImmediate(_textureImage, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
			                                             VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) == false)
			{
				goto exit;
			}

			if (renderer->CreateImageView(_textureImage, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, &_textureImageView) == false)
			{
				goto exit;
			}

			ret = true;

		exit:

			if (ret == false)
			{
				if (_textureSampler != VK_NULL_HANDLE)
				{
					vkDestroySampler(renderer->GetVkDevice(), _textureSampler, nullptr);
					_textureSampler = VK_NULL_HANDLE;
				}

				if (_textureImageView != VK_NULL_HANDLE)
				{
					vkDestroyImageView(renderer->GetVkDevice(), _textureImageView, nullptr);
					_textureImageView = VK_NULL_HANDLE;
				}

				if (_textureImage != VK_NULL_HANDLE)
				{
					vkDestroyImage(renderer->GetVkDevice(), _textureImage, nullptr);
					_textureImage = VK_NULL_HANDLE;
				}

				if (_textureImageMemory != VK_NULL_HANDLE)
				{
					vkFreeMemory(renderer->GetVkDevice(), _textureImageMemory, nullptr);
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
			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			VkBuffer       buffer = VK_NULL_HANDLE;
			VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
			bool           ret = false;

			VkMemoryPropertyFlags memoryPropertyFlags =
				createInfo._allowReadWrite ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			VkImageTiling     imageTiling = createInfo._allowReadWrite ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;
			VkImageUsageFlags imageUseFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			if (createInfo._allowReadWrite)
			{
				imageUseFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			}

			SamplerCreateInfo samplerCreateInfo;
			samplerCreateInfo._wrapMode = createInfo._wrapMode;
			samplerCreateInfo._filterMode = createInfo._filterMode;

			if (renderer->CreateImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, imageTiling, imageUseFlags, memoryPropertyFlags, &_textureImage, &_textureImageMemory) == false)
			{
				goto exit;
			}

			if (renderer->TransitionImageLayoutImmediate(_textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) == false)
			{
				goto exit;
			}

			if (renderer->CreateImageView(_textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &_textureImageView) == false)
			{
				goto exit;
			}

			if (renderer->CreateSampler(&_textureSampler, samplerCreateInfo) == false)
			{
				goto exit;
			}

			ret = true;

		exit:
			if (buffer != VK_NULL_HANDLE)
			{
				vkDestroyBuffer(renderer->GetVkDevice(), buffer, nullptr);
			}
			if (bufferMemory != VK_NULL_HANDLE)
			{
				vkFreeMemory(renderer->GetVkDevice(), bufferMemory, nullptr);
			}

			if (ret == false)
			{
				if (_textureSampler != VK_NULL_HANDLE)
				{
					vkDestroySampler(renderer->GetVkDevice(), _textureSampler, nullptr);
					_textureSampler = VK_NULL_HANDLE;
				}

				if (_textureImageView != VK_NULL_HANDLE)
				{
					vkDestroyImageView(renderer->GetVkDevice(), _textureImageView, nullptr);
					_textureImageView = VK_NULL_HANDLE;
				}

				if (_textureImage != VK_NULL_HANDLE)
				{
					vkDestroyImage(renderer->GetVkDevice(), _textureImage, nullptr);
					_textureImage = VK_NULL_HANDLE;
				}

				if (_textureImageMemory != VK_NULL_HANDLE)
				{
					vkFreeMemory(renderer->GetVkDevice(), _textureImageMemory, nullptr);
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
			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

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

			if (renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &buffer,
			                           &bufferMemory) == false)
			{
				goto exit;
			}

			if (pixels != nullptr)
			{
				if (vkMapMemory(renderer->GetVkDevice(), bufferMemory, 0, bufferSize, 0, &data) != VK_SUCCESS)
				{
					OUTPUT_ERROR("Vulkan: Texture, unable to map memory");
					goto exit;
				}
				memcpy(data, pixels, static_cast<size_t>(bufferSize));
				vkUnmapMemory(renderer->GetVkDevice(), bufferMemory);
			}

			if (renderer->CreateImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, imageTiling, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, memoryPropertyFlags,
			                          &_textureImage, &_textureImageMemory) == false)
			{
				goto exit;
			}

			if (renderer->TransitionImageLayoutImmediate(_textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) == false)
			{
				goto exit;
			}

			if (renderer->CopyBufferToImage(buffer, _textureImage, width, height) == false)
			{
				goto exit;
			}

			if (renderer->TransitionImageLayoutImmediate(_textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			                                             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) == false)
			{
				goto exit;
			}

			if (renderer->CreateImageView(_textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &_textureImageView) == false)
			{
				goto exit;
			}

			if (renderer->CreateSampler(&_textureSampler, samplerCreateInfo) == false)
			{
				goto exit;
			}

			ret = true;

		exit:
			if (buffer != VK_NULL_HANDLE)
			{
				vkDestroyBuffer(renderer->GetVkDevice(), buffer, nullptr);
			}
			if (bufferMemory != VK_NULL_HANDLE)
			{
				vkFreeMemory(renderer->GetVkDevice(), bufferMemory, nullptr);
			}

			if (ret == false)
			{
				if (_textureSampler != VK_NULL_HANDLE)
				{
					vkDestroySampler(renderer->GetVkDevice(), _textureSampler, nullptr);
					_textureSampler = VK_NULL_HANDLE;
				}

				if (_textureImageView != VK_NULL_HANDLE)
				{
					vkDestroyImageView(renderer->GetVkDevice(), _textureImageView, nullptr);
					_textureImageView = VK_NULL_HANDLE;
				}

				if (_textureImage != VK_NULL_HANDLE)
				{
					vkDestroyImage(renderer->GetVkDevice(), _textureImage, nullptr);
					_textureImage = VK_NULL_HANDLE;
				}

				if (_textureImageMemory != VK_NULL_HANDLE)
				{
					vkFreeMemory(renderer->GetVkDevice(), _textureImageMemory, nullptr);
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

			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			VkBuffer      stagingBuffer;
			VmaAllocation stagingBufferAllocation;

			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.size = _width * _height * 4;
			bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo allocCreateInfo = {};
			allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
			if (vmaCreateBuffer(renderer->GetVmaAllocator(), &bufferCreateInfo, &allocCreateInfo, &stagingBuffer, &stagingBufferAllocation, nullptr) != VK_SUCCESS)
			{
				// todo output
				return Color(0.0f, 0.0f, 0.0f, 0.0f);
			}

			renderer->TransitionImageLayoutImmediate(_textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
			if (renderer->CopyImageToBuffer(_textureImage, stagingBuffer, _width, _height) == false)
			{
				// todo ouput
				return Color(0.0f, 0.0f, 0.0f, 0.0f);
			}
			renderer->TransitionImageLayoutImmediate(_textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			void* data;
			if (vmaMapMemory(renderer->GetVmaAllocator(), stagingBufferAllocation, &data) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Texture, unable to map memory");
				return Color(0.0f, 0.0f, 0.0f, 0.0f);
			}

			uint8_t* pixelData = (uint8_t*)data + ((uint32_t)position.GetY() * _width + (uint32_t)position.GetX()) * 4;

			Color color(((float)pixelData[0]) / 255.0f, ((float)pixelData[1]) / 255.0f, ((float)pixelData[2]) / 255.0f, ((float)pixelData[3]) / 255.0f);

			vmaUnmapMemory(renderer->GetVmaAllocator(), stagingBufferAllocation);
			vmaDestroyBuffer(renderer->GetVmaAllocator(), stagingBuffer, stagingBufferAllocation);

			return color;
		}
	}
}
