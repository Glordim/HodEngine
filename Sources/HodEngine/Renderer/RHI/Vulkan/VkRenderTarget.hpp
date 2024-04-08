#pragma once

#include "HodEngine/Renderer/RHI/RenderTarget.hpp"

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

			bool			Init(size_t width, size_t height, bool allowReadWrite = false) override;

			VkRenderPass	GetRenderPass() const;
			VkFramebuffer	GetFrameBuffer() const;

			void			PrepareForWrite() override;
			void			PrepareForRead() override;

		protected:

			void			Clear() override;

		private:

			VkRenderPass	_renderPass = VK_NULL_HANDLE;
			VkFramebuffer	_frameBuffer = VK_NULL_HANDLE;
		};
	}
}
