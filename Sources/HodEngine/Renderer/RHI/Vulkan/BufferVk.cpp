#include "HodEngine/Renderer/RHI/Vulkan/BufferVk.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"

#include <HodEngine/Core/Output.hpp>

namespace hod
{
	namespace renderer
	{
		VkBufferUsageFlags BufferVk::_usageMap[Usage::Count] = {
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
		};

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		BufferVk::BufferVk(Usage usage)
			: Buffer(usage)
		{
		
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		BufferVk::~BufferVk()
		{
			Release();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void BufferVk::Release()
		{
			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			if (_vkBuffer != VK_NULL_HANDLE)
			{
				vkDestroyBuffer(renderer->GetVkDevice(), _vkBuffer, nullptr);
				_vkBuffer = VK_NULL_HANDLE;
			}

			if (_vkDeviceMemory != VK_NULL_HANDLE)
			{
				vkFreeMemory(renderer->GetVkDevice(), _vkDeviceMemory, nullptr);
				_vkDeviceMemory = VK_NULL_HANDLE;
			}
			
			_size = 0;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkBuffer BufferVk::GetVkBuffer() const
		{
			return _vkBuffer;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool BufferVk::Resize(uint32_t size)
		{
			Release();

			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			VkBufferCreateInfo bufferInfo = {};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = _usageMap[_usage];
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateBuffer(renderer->GetVkDevice(), &bufferInfo, nullptr, &_vkBuffer) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to create buffer!");
				return false;
			}

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(renderer->GetVkDevice(), _vkBuffer, &memRequirements);

			uint32_t memoryTypeIndex = 0;
			if (renderer->FindMemoryTypeIndex(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &memoryTypeIndex) == false)
			{
				OUTPUT_ERROR("Vulkan: Unable to find memory type for this buffer!");
				Release();
				return false;
			}

			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = memoryTypeIndex;

			if (vkAllocateMemory(renderer->GetVkDevice(), &allocInfo, nullptr, &_vkDeviceMemory) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to allocate buffer memory!");
				Release();
				return false;
			}

			if (vkBindBufferMemory(renderer->GetVkDevice(), _vkBuffer, _vkDeviceMemory, 0) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to bind buffer and buffer memory!");
				Release();
				return false;
			}

			_size = size;
			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void* BufferVk::Lock()
		{
			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			void* data;
			if (vkMapMemory(renderer->GetVkDevice(), _vkDeviceMemory, 0, _size, 0, &data) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to map buffer memory!");
				return nullptr;
			}

			return data;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void BufferVk::Unlock()
		{
			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			vkUnmapMemory(renderer->GetVkDevice(), _vkDeviceMemory);
		}
	}
}
