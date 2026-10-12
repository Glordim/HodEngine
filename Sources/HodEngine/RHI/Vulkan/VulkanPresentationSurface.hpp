#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/PresentationSurface.hpp"

#include "HodEngine/Core/Vector.hpp"

#include <vulkan/vulkan.h>

#include <queue>

namespace hod::inline rhi
{
	class InstanceExtensionCollector;
	class DeviceExtensionCollector;
	class Texture;

	/// @brief
	class HOD_RHI_API VulkanPresentationSurface : public PresentationSurface
	{
	public:
		static bool _hasSurfaceMaintenance1;
		static bool _hasSwapchainMaintenance1;

		static bool CollectInstanceExtensionRequirements(InstanceExtensionCollector& instanceExtensionCollector);
		static bool CollectDeviceExtensionRequirements(DeviceExtensionCollector& deviceExtensionCollector);

	public:
		VulkanPresentationSurface(Window* window);
		~VulkanPresentationSurface() override;

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

		struct RetiredSemaphore
		{
			VkSemaphore semaphore;
			uint32_t    _presentCountForRemoval;
		};

		std::deque<RetiredSwapchain>  _deletionQueue;
		std::deque<RetiredSemaphore>  _semaphoreDeletionQueue;

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

		// One depth buffer serves every image of the swap chain: they are drawn into one after the other
		Texture* _depthTexture = nullptr;

		uint32_t _currentImageIndex = 0;

		uint32_t _presentCount = 0;
	};
}
