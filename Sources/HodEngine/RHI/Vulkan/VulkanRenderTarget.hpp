#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/RenderTarget.hpp"

#include <vulkan/vulkan.h>

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RHI_API VulkanRenderTarget : public RenderTarget
	{
	public:
		VulkanRenderTarget();
		~VulkanRenderTarget() override;

		bool Init(uint32_t width, uint32_t height, const Texture::CreateInfo& createInfo) override;

		VkRenderPass  GetRenderPass() const;
		VkFramebuffer GetFrameBuffer() const;

		void PrepareForWrite(const CommandBuffer* commandBuffer) override;
		void PrepareForRead(const CommandBuffer* commandBuffer) override;

	protected:
		void Clear() override;

	private:
		VkRenderPass _renderPass = VK_NULL_HANDLE;

		VkFramebuffer _frameBuffer = VK_NULL_HANDLE;
	};
}
