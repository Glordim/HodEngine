#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/RenderTarget.hpp"

#include <vulkan/vulkan.h>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_RENDERER_API VkRenderTarget : public RenderTarget
		{
		public:

							VkRenderTarget();
							~VkRenderTarget() override;

			bool			Init(uint32_t width, uint32_t height, const Texture::CreateInfo& createInfo) override;

			VkRenderPass	GetRenderPass() const;
			VkFramebuffer	GetFrameBuffer() const;

			void			PrepareForWrite(const CommandBuffer* commandBuffer) override;
			void			PrepareForRead(const CommandBuffer* commandBuffer) override;

		protected:

			void			Clear() override;

		private:

			VkRenderPass	_renderPass = VK_NULL_HANDLE;
			VkFramebuffer	_frameBuffer = VK_NULL_HANDLE;
		};
	}
}
