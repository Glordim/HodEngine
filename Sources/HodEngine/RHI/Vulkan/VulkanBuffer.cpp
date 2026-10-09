#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Vulkan/VulkanBuffer.hpp"
#include "HodEngine/RHI/Vulkan/VulkanRhiDevice.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <vk_mem_alloc.h>

namespace hod::inline rhi
{
	VkBufferUsageFlags VulkanBuffer::_usageMap[Usage::Count] = {VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT};

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VulkanBuffer::VulkanBuffer(Usage usage, uint32_t Size)
	: Buffer(usage, Size)
	{
		Resize(Size);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VulkanBuffer::~VulkanBuffer()
	{
		Release();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void VulkanBuffer::Release()
	{
		VulkanRhiDevice* rhiDevice = (VulkanRhiDevice*)RhiDevice::GetInstance();

		if (_vkBuffer != VK_NULL_HANDLE)
		{
			vmaDestroyBuffer(rhiDevice->GetVmaAllocator(), _vkBuffer, _vmaAllocation);
			_vkBuffer = VK_NULL_HANDLE;
			_vmaAllocation = VK_NULL_HANDLE;
		}

		_size = 0;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VkBuffer VulkanBuffer::GetVkBuffer() const
	{
		return _vkBuffer;
	}

	VmaAllocation VulkanBuffer::GetVmaAllocation() const
	{
		return _vmaAllocation;
	}

	void VulkanBuffer::Detach()
	{
		_vkBuffer      = VK_NULL_HANDLE;
		_vmaAllocation = VK_NULL_HANDLE;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool VulkanBuffer::Resize(uint32_t Size)
	{
		Release();

		VulkanRhiDevice* rhiDevice = (VulkanRhiDevice*)RhiDevice::GetInstance();

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = Size;
		bufferInfo.usage = _usageMap[_usage];
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		if (vmaCreateBuffer(rhiDevice->GetVmaAllocator(), &bufferInfo, &allocInfo, &_vkBuffer, &_vmaAllocation, nullptr) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create buffer!");
			return false;
		}

		_size = Size;
		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void* VulkanBuffer::Lock()
	{
		VulkanRhiDevice* rhiDevice = (VulkanRhiDevice*)RhiDevice::GetInstance();

		void* data;
		if (vmaMapMemory(rhiDevice->GetVmaAllocator(), _vmaAllocation, &data) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to map buffer memory!");
			return nullptr;
		}

		return data;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void VulkanBuffer::Unlock()
	{
		VulkanRhiDevice* rhiDevice = (VulkanRhiDevice*)RhiDevice::GetInstance();

		vmaUnmapMemory(rhiDevice->GetVmaAllocator(), _vmaAllocation);
	}
}
