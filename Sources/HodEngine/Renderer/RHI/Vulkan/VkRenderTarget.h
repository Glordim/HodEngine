#pragma once

#include "HodEngine/Renderer/RHI/RenderTarget.h"

#include <vulkan/vulkan.h>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class VkRenderTarget : public RenderTarget
		{
		public:

							VkRenderTarget();
							~VkRenderTarget() override;

			bool			Init(size_t width, size_t height) override;

			VkRenderPass	GetRenderPass() const;
			VkFramebuffer	GetFrameBuffer() const;

		protected:

			void			Clear() override;

		private:

			VkRenderPass	_renderPass = VK_NULL_HANDLE;
			VkFramebuffer	_frameBuffer = VK_NULL_HANDLE;
		};
	}
}
