#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/PresentationSurface.hpp"

#include "HodEngine/Core/Vector.hpp"

#include <vulkan/vulkan.h>

#include <queue>

namespace hod::inline renderer
{
	class InstanceExtensionCollector;
	class DeviceExtensionCollector;

	/// @brief
	class HOD_RENDERER_API VkPresentationSurface : public PresentationSurface
	{
	public:
		static bool _hasSurfaceMaintenance1;
		static bool _hasSwapchainMaintenance1;

		static bool CollectInstanceExtensionRequirements(InstanceExtensionCollector& instanceExtensionCollector);
		static bool CollectDeviceExtensionRequirements(DeviceExtensionCollector& deviceExtensionCollector);

	public:
		VkPresentationSurface(Window* window);
		~VkPresentationSurface() override;

	public:
		void    Resize(uint32_t width, uint32_t height) override;
		Vector2 GetResolution() const override;

		bool AcquireNextImageIndex(Semaphore* imageAvailableSemaphore) override;
		bool SwapBuffer() override;

		VkSurfaceKHR  GetSurface() const;
		VkRenderPass  GetRenderPass() const;
		VkExtent2D    GetSwapChainExtent() const;
		VkFramebuffer GetSwapChainCurrentFrameBuffer() const;

	protected:
		VkSurfaceKHR _surface = VK_NULL_HANDLE;

	private:
		struct RetiredSwapchain
		{
			VkSwapchainKHR        swapchain;
			Vector<VkImageView>   imageViews; // Les vues associées doivent mourir avec la swapchain
			Vector<VkFramebuffer> framebuffers;
			uint32_t              _presentCountForRemoval;
		};

		std::deque<RetiredSwapchain> _deletionQueue;

	private:
		bool CreateSurface(Window* window);

		bool CreateSwapChain(uint32_t width, uint32_t height);
		void DestroySwapChain();

	private:
		VkExtent2D            _swapChainExtent;
		VkSwapchainKHR        _swapchain = VK_NULL_HANDLE;
		Vector<VkImage>       _swapchainImages;
		Vector<VkImageView>   _swapchainImageViews;
		Vector<VkFramebuffer> _swapchainFramebuffers;
		VkRenderPass          _renderPass = VK_NULL_HANDLE;

		uint32_t _currentImageIndex = 0;

		uint32_t _presentCount = 0;
	};
}
