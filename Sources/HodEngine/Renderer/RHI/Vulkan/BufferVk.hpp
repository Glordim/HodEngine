#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/Buffer.hpp"

#include <vulkan/vulkan.h>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_RENDERER_API BufferVk : public Buffer
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
