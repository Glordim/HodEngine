#pragma once

#include "HodEngine/Renderer/RHI/Buffer.h"

#include <vulkan/vulkan.h>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class BufferVk : public Buffer
		{
		public:

										BufferVk(Usage usage);
										BufferVk(const BufferVk&) = delete;
										BufferVk(BufferVk&&) = delete;
										~BufferVk() override;

			void						operator=(const BufferVk&) = delete;
			void						operator=(BufferVk&&) = delete;

		public:

			VkBuffer					GetVkBuffer() const;

			bool						Resize(uint32_t size) override;
			void*						Lock() override;
			void						Unlock() override;

		private:

			void						Release();

		private:

			VkBuffer					_vkBuffer = VK_NULL_HANDLE;
			VkDeviceMemory				_vkDeviceMemory = VK_NULL_HANDLE;

		private:

			static VkBufferUsageFlags	_usageMap[Usage::Count];
		};
	}
}