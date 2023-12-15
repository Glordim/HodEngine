#pragma once

#include "HodEngine/Renderer/RHI/Context.hpp"

#include <vector>

#include <vulkan/vulkan.h>

namespace hod
{
	/// @brief 
	class VkContext : public renderer::Context
	{
	public:

									VkContext(VkSurfaceKHR surface);
									~VkContext() override;

	public:

		void						Resize(uint32_t width, uint32_t height) override;

		bool						AcquireNextImageIndex() override;
		bool						SwapBuffer() override;

		VkSurfaceKHR				GetSurface() const;
		VkRenderPass				GetRenderPass() const;
		VkExtent2D					GetSwapChainExtent() const;
		VkFramebuffer				GetSwapChainCurrentFrameBuffer() const;

	private:

		bool						CreateSwapChain(uint32_t width, uint32_t height);
		void						DestroySwapChain();

		bool						CreateSemaphores();

	protected:

		VkSurfaceKHR				_surface = VK_NULL_HANDLE;

	private:

		VkExtent2D					_swapChainExtent;
		VkSwapchainKHR				_swapchain = VK_NULL_HANDLE;
		std::vector<VkImageView>	_swapchainImageViews;
		std::vector<VkFramebuffer>	_swapchainFramebuffers;
		VkRenderPass				_renderPass = VK_NULL_HANDLE;

		uint32_t					_currentImageIndex = 0;
		VkSemaphore                 _imageAvailableSemaphore = VK_NULL_HANDLE;
		VkSemaphore                 _renderFinishedSemaphore = VK_NULL_HANDLE;
		VkFence                     _acquireNextImageFence = VK_NULL_HANDLE;
	};
}
