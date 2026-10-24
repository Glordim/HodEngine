#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Vulkan/VulkanRenderTarget.hpp"

#include "HodEngine/RHI/Vulkan/VulkanCommandBuffer.hpp"
#include "HodEngine/RHI/Vulkan/VulkanRhiDevice.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>


namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VulkanRenderTarget::VulkanRenderTarget()
	: RenderTarget()
	{
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VulkanRenderTarget::~VulkanRenderTarget()
	{
		Clear();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool VulkanRenderTarget::Init(uint32_t width, uint32_t height, bool readback, PixelFormat colorFormat, PixelFormat depthFormat)
	{
		Clear();

		if (RenderTarget::Init(width, height, readback, colorFormat, depthFormat) == false)
		{
			return false;
		}

		VulkanRhiDevice* rhiDevice = VulkanRhiDevice::GetInstance();

		if (rhiDevice->CreateRenderPass(_colorFormat, _depthFormat, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, &_renderPass) == false)
		{
			Clear();
			return false;
		}

		// In the order of the attachments of the render pass
		VkImageView attachmentImageViews[2] = {};
		uint32_t    attachmentCount = 0;
		if (_colorTexture != nullptr)
		{
			attachmentImageViews[attachmentCount++] = static_cast<VulkanTexture*>(_colorTexture)->GetTextureImageView();
		}
		if (_depthTexture != nullptr)
		{
			attachmentImageViews[attachmentCount++] = static_cast<VulkanTexture*>(_depthTexture)->GetTextureImageView();
		}

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = _renderPass;
		framebufferInfo.attachmentCount = attachmentCount;
		framebufferInfo.pAttachments = attachmentImageViews;
		framebufferInfo.width = width;
		framebufferInfo.height = height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(rhiDevice->GetVkDevice(), &framebufferInfo, nullptr, &_frameBuffer) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create Framebuffer !");
			Clear();
			return false;
		}

		return true;
	}

	/// @brief
	void VulkanRenderTarget::Clear()
	{
		RenderTarget::Clear();

		VulkanRhiDevice* rhiDevice = VulkanRhiDevice::GetInstance();

		if (_frameBuffer != VK_NULL_HANDLE)
		{
			rhiDevice->DeferDestroy(_frameBuffer);
			_frameBuffer = VK_NULL_HANDLE;
		}

		if (_renderPass != VK_NULL_HANDLE)
		{
			rhiDevice->DeferDestroy(_renderPass);
			_renderPass = VK_NULL_HANDLE;
		}
	}

	/// @brief
	/// @return
	VkRenderPass VulkanRenderTarget::GetRenderPass() const
	{
		return _renderPass;
	}

	/// @brief
	/// @return
	VkFramebuffer VulkanRenderTarget::GetFrameBuffer() const
	{
		return _frameBuffer;
	}

	/// @brief
	void VulkanRenderTarget::PrepareForWrite(const CommandBuffer* commandBuffer)
	{
		VulkanRhiDevice* rhiDevice = VulkanRhiDevice::GetInstance();

		VkCommandBuffer vkCommandBuffer = static_cast<const VulkanCommandBuffer*>(commandBuffer)->GetVkCommandBuffer();

		Texture* colorTexture = GetColorTexture();

		if (colorTexture != nullptr && rhiDevice->TransitionImageLayout(vkCommandBuffer, static_cast<VulkanTexture*>(colorTexture)->GetTextureImage(), VK_IMAGE_ASPECT_COLOR_BIT,
																		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) == false)
		{
			return; // todo bool ?
		}
	}

	/// @brief
	void VulkanRenderTarget::PrepareForRead(const CommandBuffer* commandBuffer)
	{
		VulkanRhiDevice* rhiDevice = VulkanRhiDevice::GetInstance();

		VkCommandBuffer vkCommandBuffer = static_cast<const VulkanCommandBuffer*>(commandBuffer)->GetVkCommandBuffer();

		Texture* colorTexture = GetColorTexture();

		if (colorTexture != nullptr && rhiDevice->TransitionImageLayout(vkCommandBuffer, static_cast<VulkanTexture*>(colorTexture)->GetTextureImage(), VK_IMAGE_ASPECT_COLOR_BIT,
																		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) == false)
		{
			return; // todo bool ?
		}
	}
}
