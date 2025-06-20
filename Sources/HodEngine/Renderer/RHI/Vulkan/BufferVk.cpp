#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/BufferVk.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <VulkanMemoryAllocator/vk_mem_alloc.h>

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
		BufferVk::BufferVk(Usage usage, uint32_t size)
			: Buffer(usage, size)
		{
			Resize(size);
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
				vmaDestroyBuffer(renderer->GetVmaAllocator(), _vkBuffer, _vmaAllocation);
				_vkBuffer = VK_NULL_HANDLE;
				_vmaAllocation = VK_NULL_HANDLE;
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

			VmaAllocationCreateInfo allocInfo = {};
			allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

			if (vmaCreateBuffer(renderer->GetVmaAllocator(), &bufferInfo, &allocInfo, &_vkBuffer, &_vmaAllocation, nullptr) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to create buffer!");
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
			if (vmaMapMemory(renderer->GetVmaAllocator(), _vmaAllocation, &data) != VK_SUCCESS)
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

			vmaUnmapMemory(renderer->GetVmaAllocator(), _vmaAllocation);
		}
	}
}
