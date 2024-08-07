#include "HodEngine/Renderer/RHI/Vulkan/VkTexture.hpp"

#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <cstring>

#include <HodEngine/Core/Math/Vector2.hpp>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkTexture::VkTexture() : Texture()
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
		bool VkTexture::BuildDepth(uint32_t width, uint32_t height, const CreateInfo& createInfo)
		{
			bool ret = false;

			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			VkMemoryPropertyFlags memoryPropertyFlags = /*createInfo._allowReadWrite ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : */VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			VkImageTiling imageTiling = /*createInfo._allowReadWrite ? VK_IMAGE_TILING_LINEAR : */VK_IMAGE_TILING_OPTIMAL;

			if (renderer->CreateImage(width, height, VK_FORMAT_D32_SFLOAT_S8_UINT, imageTiling, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, memoryPropertyFlags, &_textureImage, &_textureImageMemory) == false)
			{
				goto exit;
			}

			if (renderer->TransitionImageLayout(_textureImage, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) == false)
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

			return ret;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool VkTexture::BuildColor(uint32_t width, uint32_t height, const CreateInfo& createInfo)
		{
			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			VkBuffer buffer = VK_NULL_HANDLE;
			VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
			bool ret = false;

			VkMemoryPropertyFlags memoryPropertyFlags = createInfo._allowReadWrite ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			VkImageTiling imageTiling = createInfo._allowReadWrite ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;

			SamplerCreateInfo samplerCreateInfo;
			samplerCreateInfo._wrapMode = createInfo._wrapMode;
			samplerCreateInfo._filterMode = createInfo._filterMode;

			if (renderer->CreateImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, imageTiling, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, memoryPropertyFlags, &_textureImage, &_textureImageMemory) == false)
			{
				goto exit;
			}

			if (renderer->TransitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) == false)
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

			VkBuffer buffer = VK_NULL_HANDLE;
			VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
			VkDeviceSize bufferSize = width * height * 4;
			void* data = nullptr;
			bool ret = false;

			VkMemoryPropertyFlags memoryPropertyFlags = createInfo._allowReadWrite ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			VkImageTiling imageTiling = createInfo._allowReadWrite ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;

			SamplerCreateInfo samplerCreateInfo;
			samplerCreateInfo._wrapMode = createInfo._wrapMode;
			samplerCreateInfo._filterMode = createInfo._filterMode;

			if (renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &buffer, &bufferMemory) == false)
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

			if (renderer->CreateImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, imageTiling, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, memoryPropertyFlags, &_textureImage, &_textureImageMemory) == false)
			{
				goto exit;
			}

			if (renderer->TransitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) == false)
			{
				goto exit;
			}

			if (renderer->CopyBufferToImage(buffer, _textureImage, width, height) == false)
			{
				goto exit;
			}

			if (renderer->TransitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) == false)
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
			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			void* data;
			if (vkMapMemory(renderer->GetVkDevice(), _textureImageMemory, 0, VK_WHOLE_SIZE, 0, &data) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Texture, unable to map memory");
				return Color(0.0f, 0.0f, 0.0f, 0.0f);
			}
			
			//VkMemoryRequirements memRequirements;
			//vkGetImageMemoryRequirements(renderer->GetVkDevice(), _textureImage, &memRequirements);
			size_t alignment = renderer->GetVkGpuDevice()->deviceProperties.limits.minMemoryMapAlignment; // memRequirements.alignment

			VkDeviceSize lineSizeWithAlignment = 4 * _width;
			lineSizeWithAlignment += alignment - (lineSizeWithAlignment % alignment);

			VkDeviceSize bufferOffset = lineSizeWithAlignment * (VkDeviceSize)position.GetY() + 4 * (VkDeviceSize)position.GetX(); 

			Color color;
			color.r = reinterpret_cast<uint8_t*>(data)[bufferOffset + 0];
			color.g = reinterpret_cast<uint8_t*>(data)[bufferOffset + 1];
			color.b = reinterpret_cast<uint8_t*>(data)[bufferOffset + 2];
			color.a = reinterpret_cast<uint8_t*>(data)[bufferOffset + 3];

			color.r /= 255.0f;
			color.g /= 255.0f;
			color.b /= 255.0f;
			color.a /= 255.0f;
			vkUnmapMemory(renderer->GetVkDevice(), _textureImageMemory);

			return color;
		}
	}
}

