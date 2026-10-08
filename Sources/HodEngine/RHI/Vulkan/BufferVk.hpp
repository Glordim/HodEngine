#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Buffer.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_RHI_API BufferVk : public Buffer
	{
	public:

									BufferVk(Usage usage, uint32_t size);
									BufferVk(const BufferVk&) = delete;
									BufferVk(BufferVk&&) = delete;
									~BufferVk() override;

		BufferVk&					operator=(const BufferVk&) = delete;
		BufferVk&					operator=(BufferVk&&) = delete;

	public:

		VkBuffer					GetVkBuffer() const;
		VmaAllocation				GetVmaAllocation() const;
		void						Detach(); // nulls handles so destructor skips destroy (use with DeferDestroy)

		bool						Resize(uint32_t size) override;
		void*						Lock() override;
		void						Unlock() override;

	private:

		void						Release();

	private:

		VkBuffer					_vkBuffer = VK_NULL_HANDLE;
		VmaAllocation				_vmaAllocation = VK_NULL_HANDLE;

	private:

		static VkBufferUsageFlags	_usageMap[Usage::Count];
	};
}
