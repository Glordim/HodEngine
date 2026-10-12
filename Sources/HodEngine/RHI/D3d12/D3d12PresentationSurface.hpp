#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/PresentationSurface.hpp"

#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/RHI/D3d12/D3d12Api.hpp"
#include "HodEngine/RHI/D3d12/D3d12DescriptorHeap.hpp"

namespace hod::inline rhi
{
	class Texture;

	/// @brief The swap chain of a window
	class HOD_RHI_API D3d12PresentationSurface : public PresentationSurface
	{
	public:
		D3d12PresentationSurface(window::Window* window);
		~D3d12PresentationSurface() override;

	public:
		void    Resize(uint32_t width, uint32_t height) override;
		Vector2 GetResolution() const override;

		bool AcquireNextImageIndex(Semaphore* imageAvailableSemaphore) override;
		bool SwapBuffer() override;

		// False as long as the window never had a size, which leaves it without a swap chain
		bool IsValid() const;

		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;
		void                        TransitionCurrentBackBuffer(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES state);

	private:
		struct BackBuffer
		{
			ComPtr<ID3D12Resource> _resource;
			uint32_t               _renderTargetViewIndex = D3d12DescriptorHeap::InvalidIndex;
			D3D12_RESOURCE_STATES  _state = D3D12_RESOURCE_STATE_PRESENT;
		};

	private:
		bool CreateSwapChain(uint32_t width, uint32_t height);
		bool AcquireBackBuffers();
		void ReleaseBackBuffers();
		bool CreateDepthTexture();

	private:
		ComPtr<IDXGISwapChain3> _swapChain;
		Vector<BackBuffer>      _backBuffers;

		// One depth buffer serves every back buffer: they are drawn into one after the other
		Texture* _depthTexture = nullptr;

		uint32_t _width = 0;
		uint32_t _height = 0;

		uint32_t _currentImageIndex = 0;
	};
}
