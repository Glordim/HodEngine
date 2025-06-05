#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/Context.hpp"

#include "HodEngine/Core/Vector.hpp"

namespace hod::window
{
	class MacOsWindow;
}

namespace CA
{
	class MetalLayer;
	class MetalDrawable;
}

namespace hod::renderer
{
	/// @brief 
	class HOD_RENDERER_API MetalContext : public Context
	{
	public:

									MetalContext(window::MacOsWindow* window);
									~MetalContext() override;

	public:

		bool						AcquireNextImageIndex(const Semaphore* signalSemaphore) override;
		CA::MetalDrawable*			GetCurrentDrawable() const;

		void						Resize(uint32_t width, uint32_t height) override;
		uint32_t					GetWidth() override;
		uint32_t					GetHeight() override;

		bool						SwapBuffer(const Semaphore* waitSemaphore) override;

		void						Present();

		CA::MetalLayer*				GetLayer() const;

/*
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
		Vector<VkImageView>	_swapchainImageViews;
		Vector<VkFramebuffer>	_swapchainFramebuffers;
		VkRenderPass				_renderPass = VK_NULL_HANDLE;

		uint32_t					_currentImageIndex = 0;
		VkSemaphore                 _imageAvailableSemaphore = VK_NULL_HANDLE;
		VkSemaphore                 _renderFinishedSemaphore = VK_NULL_HANDLE;
		VkFence                     _acquireNextImageFence = VK_NULL_HANDLE;
*/
	private:

		void						SetupLayer(window::MacOsWindow* window);

	private:

		CA::MetalLayer*				_layer = nullptr;
		CA::MetalDrawable*			_drawable = nullptr;

		window::MacOsWindow*		_window = nullptr;
	};
}
