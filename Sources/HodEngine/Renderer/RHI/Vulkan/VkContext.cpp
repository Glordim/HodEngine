#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/SemaphoreVk.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkContext.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <limits>

namespace hod::renderer
{
	/// @brief
	VkContext::VkContext(VkSurfaceKHR surface)
	: Context()
	, _surface(surface)
	{
		CreateSwapChain(800, 600);
	}

	/// @brief
	VkContext::~VkContext()
	{
		RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

		DestroySwapChain();

		if (_surface != VK_NULL_HANDLE)
		{
			vkDestroySurfaceKHR(renderer->GetVkInstance(), _surface, nullptr);
		}
	}

	/// @brief
	/// @param width
	/// @param height
	void VkContext::Resize(uint32_t width, uint32_t height)
	{
		CreateSwapChain(width, height);
	}

	/// @brief
	/// @return
	Vector2 VkContext::GetResolution()
	{
		return Vector2((float)_swapChainExtent.width, (float)_swapChainExtent.height);
	}

	/// @brief
	/// @return
	bool VkContext::CreateSwapChain(uint32_t width, uint32_t height)
	{
		VkDevice device = RendererVulkan::GetInstance()->GetVkDevice();
		if (device == nullptr)
		{
			return true;
		}
		vkDeviceWaitIdle(device);

		DestroySwapChain();

		_currentImageIndex = 0;

		VkSurfaceCapabilitiesKHR   capabilities;
		Vector<VkSurfaceFormatKHR> formats;
		Vector<VkPresentModeKHR>   presentModes;

		const VkGpuDevice* selectedGpuDevice = RendererVulkan::GetInstance()->GetVkGpuDevice();

		if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(selectedGpuDevice->physicalDevice, _surface, &capabilities) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to get Surface capabilities !");
			return false;
		}

		if (capabilities.currentExtent.width == 0)
		{
			return false;
		}

		VkExtent2D extent;

		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() && capabilities.currentExtent.height != std::numeric_limits<uint32_t>::max())
		{
			extent = capabilities.currentExtent;
		}
		else
		{
			extent.width = width;
			extent.height = height;

			// TODO Replace by clamp
			if (extent.width > capabilities.maxImageExtent.width)
			{
				extent.width = capabilities.maxImageExtent.width;
			}
			else if (extent.width < capabilities.minImageExtent.width)
			{
				extent.width = capabilities.minImageExtent.width;
			}

			// TODO Replace by clamp
			if (extent.height > capabilities.maxImageExtent.height)
			{
				extent.height = capabilities.maxImageExtent.height;
			}
			else if (extent.height < capabilities.minImageExtent.height)
			{
				extent.height = capabilities.minImageExtent.height;
			}
		}
		_swapChainExtent = extent;

		uint32_t imageCount = capabilities.minImageCount + 1;

		// TODO Replace by max
		if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
		{
			imageCount = capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = _surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
		createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		createInfo.imageExtent = _swapChainExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // Present and graphics queue have the same queue family
		createInfo.queueFamilyIndexCount = 0;                    // Optional
		createInfo.pQueueFamilyIndices = nullptr;                // Optional
		createInfo.preTransform = capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR; // Todo support VSync
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &_swapchain) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create SwapChain !");
			return false;
		}

		// Render pass
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
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

		/*
		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		*/

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = nullptr; // &depthAttachmentRef;

		VkAttachmentDescription attachments[] = {colorAttachment}; // , depthAttachment };

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = attachments;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create render pass!");
			return false;
		}

		// Image views
		vkGetSwapchainImagesKHR(device, _swapchain, &imageCount, nullptr);

		Vector<VkImage> swapChainImages(imageCount);
		vkGetSwapchainImagesKHR(device, _swapchain, &imageCount, swapChainImages.Data());

		_swapchainImageViews.Resize(imageCount, VK_NULL_HANDLE);
		_swapchainFramebuffers.Resize(imageCount, VK_NULL_HANDLE);

		for (size_t i = 0; i < imageCount; ++i)
		{
			RendererVulkan::GetInstance()->TransitionImageLayoutImmediate(swapChainImages[i], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
			                                                              VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

			// TODO use CreateImageView here ?

			VkImageViewCreateInfo imageCreateInfo = {};
			imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageCreateInfo.image = swapChainImages[i];
			imageCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
			imageCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageCreateInfo.subresourceRange.baseMipLevel = 0;
			imageCreateInfo.subresourceRange.levelCount = 1;
			imageCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageCreateInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device, &imageCreateInfo, nullptr, &_swapchainImageViews[i]) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to create Image Views !");
				return false;
			}

			VkImageView attachments[] = {
				_swapchainImageViews[i],
				//_depthTexture.GetTextureImageView()
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = _renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = _swapChainExtent.width;
			framebufferInfo.height = _swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &_swapchainFramebuffers[i]) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to create Framebuffer !");
				return false;
			}
		}

		return true;
	}

	/// @brief
	void VkContext::DestroySwapChain()
	{
		VkDevice device = RendererVulkan::GetInstance()->GetVkDevice();

		size_t swapChainFramebufferCount = _swapchainFramebuffers.Size();
		for (size_t i = 0; i < swapChainFramebufferCount; ++i)
		{
			vkDestroyFramebuffer(device, _swapchainFramebuffers[i], nullptr);
		}
		_swapchainFramebuffers.Clear();

		size_t swapChainImageViewCount = _swapchainImageViews.Size();
		for (size_t i = 0; i < swapChainImageViewCount; ++i)
		{
			vkDestroyImageView(device, _swapchainImageViews[i], nullptr);
		}
		_swapchainImageViews.Clear();

		if (_renderPass != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(device, _renderPass, nullptr);
			_renderPass = VK_NULL_HANDLE;
		}

		if (_swapchain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(device, _swapchain, nullptr);
			_swapchain = VK_NULL_HANDLE;
		}
	}

	/// @brief
	/// @return
	VkSurfaceKHR VkContext::GetSurface() const
	{
		return _surface;
	}

	/// @brief
	/// @return
	VkRenderPass VkContext::GetRenderPass() const
	{
		return _renderPass;
	}

	/// @brief
	/// @return
	VkExtent2D VkContext::GetSwapChainExtent() const
	{
		return _swapChainExtent;
	}

	/// @brief
	/// @return
	VkFramebuffer VkContext::GetSwapChainCurrentFrameBuffer() const
	{
		return _swapchainFramebuffers[_currentImageIndex];
	}

	/// @brief
	/// @return
	bool VkContext::AcquireNextImageIndex()
	{
		VkDevice device = RendererVulkan::GetInstance()->GetVkDevice();

		if (_swapchain == nullptr) // For exemple if the window is hidden the Size will be 0 and not swap chain are created
		{
			return false;
		}

		VkResult result = vkAcquireNextImageKHR(device, _swapchain, std::numeric_limits<uint64_t>::max(),
		                                        static_cast<const SemaphoreVk*>(_imageAvailableSemaphore)->GetVkSemaphore(), VK_NULL_HANDLE, &_currentImageIndex);
		if (result != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to acquire next image!");

			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				OUTPUT_ERROR("Vulkan: VK_ERROR_OUT_OF_DATE_KHR recreating SwapChain...");
				// CreateSwapChain();
			}
			else if (result == VK_SUBOPTIMAL_KHR)
			{
				OUTPUT_ERROR("Vulkan: VK_SUBOPTIMAL_KHR recreating SwapChain...");
				// CreateSwapChain();
			}

			return false;
		}

		return true;
	}

	/// @brief
	/// @return
	bool VkContext::SwapBuffer()
	{
		if (_swapchain == nullptr) // For exemple if the window is hidden the Size will be 0 and not swap chain are created
		{
			return false;
		}

		Vector<VkSemaphore> waitSemaphores;
		VkPresentInfoKHR    presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		if (_semaphoresToSwapBuffer.Empty())
		{
			presentInfo.waitSemaphoreCount = 0;
			presentInfo.pWaitSemaphores = nullptr;
		}
		else
		{
			waitSemaphores.Reserve(_semaphoresToSwapBuffer.Size());
			for (const Semaphore* semaphore : _semaphoresToSwapBuffer)
			{
				VkSemaphore vkWaitSemaphore = static_cast<const SemaphoreVk*>(semaphore)->GetVkSemaphore();
				waitSemaphores.push_back(vkWaitSemaphore);
			}
			presentInfo.waitSemaphoreCount = (uint32_t)waitSemaphores.Size();
			presentInfo.pWaitSemaphores = waitSemaphores.Data();
			_semaphoresToSwapBuffer.Clear();
		}

		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &_swapchain;
		presentInfo.pImageIndices = &_currentImageIndex;
		presentInfo.pResults = nullptr;

		VkQueue  presentQueue = RendererVulkan::GetInstance()->GetPresentQueue();
		VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);
		if (result != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to present frame!");

			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				OUTPUT_ERROR("Vulkan: VK_ERROR_OUT_OF_DATE_KHR recreating SwapChain...");
				// CreateSwapChain();
			}
			else if (result == VK_SUBOPTIMAL_KHR)
			{
				OUTPUT_ERROR("Vulkan: VK_SUBOPTIMAL_KHR recreating SwapChain...");
				// CreateSwapChain();
			}

			return false;
		}

		return true;
	}
}
