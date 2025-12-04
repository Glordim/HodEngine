#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/PresentationSurface.hpp"

#include "HodEngine/Core/Vector.hpp"

#include <vulkan/vulkan.h>

namespace hod::renderer
{
	/// @brief
	class HOD_RENDERER_API VkPresentationSurface : public PresentationSurface
	{
	public:
		VkPresentationSurface(window::Window* window, VkSurfaceKHR surface);
		~VkPresentationSurface() override;

	public:
		bool    ApplyResize() override;
		Vector2 GetResolution() const override;

		bool AcquireNextImageIndex(Semaphore* imageAvailableSemaphore) override;
		bool SwapBuffer() override;

		VkSurfaceKHR  GetSurface() const;
		VkRenderPass  GetRenderPass() const;
		VkExtent2D    GetSwapChainExtent() const;
		VkFramebuffer GetSwapChainCurrentFrameBuffer() const;

	private:
		bool CreateSwapChain(uint32_t width, uint32_t height);
		void DestroySwapChain();

	protected:
		VkSurfaceKHR _surface = VK_NULL_HANDLE;

	private:
		VkExtent2D            _swapChainExtent;
		VkSwapchainKHR        _swapchain = VK_NULL_HANDLE;
		Vector<VkImageView>   _swapchainImageViews;
		Vector<VkFramebuffer> _swapchainFramebuffers;
		VkRenderPass          _renderPass = VK_NULL_HANDLE;

		uint32_t _currentImageIndex = 0;
	};
}
