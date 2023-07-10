#include "HodEngine/Renderer/RHI/Vulkan/VkRenderTarget.h"

#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.h"

#include <HodEngine/Core/Output.h>

#include <cstring>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkRenderTarget::VkRenderTarget() : RenderTarget()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkRenderTarget::~VkRenderTarget()
		{
			Clear();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool VkRenderTarget::Init(size_t width, size_t height)
		{
			Clear();

			if (RenderTarget::Init(width, height) == false)
			{
				return false;
			}

			// Render pass
			VkAttachmentDescription colorAttachment = {};
			colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
			colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference colorAttachmentRef = {};
			colorAttachmentRef.attachment = 0;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkAttachmentDescription depthAttachment = {};
			depthAttachment.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
			depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			/*
			VkAttachmentReference depthAttachmentRef = {};
			depthAttachmentRef.attachment = 1;
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			*/

			VkSubpassDescription subpass = {};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colorAttachmentRef;
			subpass.pDepthStencilAttachment = nullptr;// &depthAttachmentRef;

			VkAttachmentDescription attachments[] = { colorAttachment }; // , depthAttachment };

			VkRenderPassCreateInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = 1;
			renderPassInfo.pAttachments = attachments;
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpass;

			RendererVulkan* renderer = RendererVulkan::GetInstance();

			if (vkCreateRenderPass(renderer->GetVkDevice(), &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to create render pass!");
				Clear();
				return false;
			}

			VkImageView attachmentImageViews[] = {
				static_cast<VkTexture*>(GetColorTexture())->GetTextureImageView(),
				//static_cast<VkTexture*>(GetDepthTexture())->GetTextureImageView()
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = _renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachmentImageViews;
			framebufferInfo.width = width;
			framebufferInfo.height = height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(renderer->GetVkDevice(), &framebufferInfo, nullptr, &_frameBuffer) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to create Framebuffer !");
				Clear();
				return false;
			}
		}

		/// @brief 
		void VkRenderTarget::Clear()
		{
			RenderTarget::Clear();

			RendererVulkan* renderer = RendererVulkan::GetInstance();

			if (_frameBuffer != VK_NULL_HANDLE)
			{
				vkDestroyFramebuffer(renderer->GetVkDevice(), _frameBuffer, nullptr);
				_frameBuffer = VK_NULL_HANDLE;
			}

			if (_renderPass != VK_NULL_HANDLE)
			{
				vkDestroyRenderPass(renderer->GetVkDevice(), _renderPass, nullptr);
				_renderPass = VK_NULL_HANDLE;
			}
		}

		/// @brief 
		/// @return 
		VkRenderPass VkRenderTarget::GetRenderPass() const
		{
			return _renderPass;
		}
		
		/// @brief 
		/// @return 
		VkFramebuffer VkRenderTarget::GetFrameBuffer() const
		{
			return _frameBuffer;
		}
	}
}
