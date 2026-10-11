#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/D3d12/D3d12PresentationSurface.hpp"

#include "HodEngine/RHI/D3d12/D3d12RhiDevice.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <HodEngine/Window/Desktop/Windows/Win32/Win32Window.hpp>

namespace hod::inline rhi
{
	namespace
	{
		constexpr DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	/// @brief
	/// @param window
	D3d12PresentationSurface::D3d12PresentationSurface(window::Window* window)
	: PresentationSurface(window)
	{
	}

	/// @brief
	D3d12PresentationSurface::~D3d12PresentationSurface()
	{
		D3d12RhiDevice* rhiDevice = D3d12RhiDevice::GetInstance();

		// The back buffers can't be kept alive past their swap chain: nothing may still be drawing into them
		rhiDevice->WaitIdle();

		ReleaseBackBuffers();
		for (const BackBuffer& backBuffer : _backBuffers)
		{
			if (backBuffer._renderTargetViewIndex != D3d12DescriptorHeap::InvalidIndex)
			{
				rhiDevice->GetRenderTargetViewHeap().Free(backBuffer._renderTargetViewIndex);
			}
		}
	}

	/// @brief
	/// @param width
	/// @param height
	void D3d12PresentationSurface::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0) // minimized or hidden: what the window had so far is kept
		{
			return;
		}

		if (_swapChain == nullptr)
		{
			CreateSwapChain(width, height);
			return;
		}

		if (width == _width && height == _height)
		{
			return;
		}

		// Resizing takes every reference to the back buffers to be gone
		D3d12RhiDevice::GetInstance()->WaitIdle();
		ReleaseBackBuffers();

		if (FAILED(_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0)))
		{
			OUTPUT_ERROR("D3d12: Unable to resize SwapChain !");
		}
		else
		{
			_width = width;
			_height = height;
		}

		AcquireBackBuffers();
	}

	/// @brief
	/// @return
	Vector2 D3d12PresentationSurface::GetResolution() const
	{
		return Vector2((float)_width, (float)_height);
	}

	/// @brief
	/// @param width
	/// @param height
	/// @return
	bool D3d12PresentationSurface::CreateSwapChain(uint32_t width, uint32_t height)
	{
		D3d12RhiDevice* rhiDevice = D3d12RhiDevice::GetInstance();
		HWND            hwnd = static_cast<Win32Window*>(_window)->GetWindowHandle();

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = BackBufferFormat;
		swapChainDesc.Stereo = FALSE;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = rhiDevice->GetFrameInFlightCount() + 1;
		swapChainDesc.Scaling = DXGI_SCALING_NONE; // while the window is resized, what was drawn is not stretched to its new size
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.Flags = 0;

		ComPtr<IDXGISwapChain1> swapChain;
		if (FAILED(rhiDevice->GetDxgiFactory()->CreateSwapChainForHwnd(rhiDevice->GetCommandQueue(), hwnd, &swapChainDesc, nullptr, nullptr, &swapChain)) ||
		    FAILED(swapChain.As(&_swapChain)))
		{
			OUTPUT_ERROR("D3d12: Unable to create SwapChain !");
			return false;
		}

		// Fullscreen is the business of the window, not of DXGI
		rhiDevice->GetDxgiFactory()->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

		_width = width;
		_height = height;

		_backBuffers.Resize(swapChainDesc.BufferCount);
		for (BackBuffer& backBuffer : _backBuffers)
		{
			backBuffer._renderTargetViewIndex = rhiDevice->GetRenderTargetViewHeap().Allocate();
			if (backBuffer._renderTargetViewIndex == D3d12DescriptorHeap::InvalidIndex)
			{
				OUTPUT_ERROR("D3d12: PresentationSurface, the render target view heap is full");
				return false;
			}
		}

		return AcquireBackBuffers();
	}

	/// @brief
	/// @return
	bool D3d12PresentationSurface::AcquireBackBuffers()
	{
		D3d12RhiDevice* rhiDevice = D3d12RhiDevice::GetInstance();

		for (uint32_t index = 0; index < _backBuffers.Size(); ++index)
		{
			BackBuffer& backBuffer = _backBuffers[index];

			if (FAILED(_swapChain->GetBuffer(index, IID_PPV_ARGS(&backBuffer._resource))))
			{
				OUTPUT_ERROR("D3d12: Unable to retreive BackBuffer from SwapChain!");
				return false;
			}
			backBuffer._state = D3D12_RESOURCE_STATE_PRESENT;

			rhiDevice->GetDevice()->CreateRenderTargetView(backBuffer._resource.Get(), nullptr,
			                                               rhiDevice->GetRenderTargetViewHeap().GetCpuHandle(backBuffer._renderTargetViewIndex));
		}

		_currentImageIndex = _swapChain->GetCurrentBackBufferIndex();
		return true;
	}

	/// @brief
	void D3d12PresentationSurface::ReleaseBackBuffers()
	{
		for (BackBuffer& backBuffer : _backBuffers)
		{
			backBuffer._resource.Reset();
		}
	}

	/// @brief
	/// @return
	bool D3d12PresentationSurface::IsValid() const
	{
		return _swapChain != nullptr && _backBuffers.Empty() == false && _backBuffers[_currentImageIndex]._resource != nullptr;
	}

	/// @brief
	/// @return
	D3D12_CPU_DESCRIPTOR_HANDLE D3d12PresentationSurface::GetCurrentRenderTargetView() const
	{
		return D3d12RhiDevice::GetInstance()->GetRenderTargetViewHeap().GetCpuHandle(_backBuffers[_currentImageIndex]._renderTargetViewIndex);
	}

	/// @brief
	/// @param commandList
	/// @param state
	void D3d12PresentationSurface::TransitionCurrentBackBuffer(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES state)
	{
		BackBuffer& backBuffer = _backBuffers[_currentImageIndex];
		if (backBuffer._state == state)
		{
			return;
		}

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = backBuffer._resource.Get();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = backBuffer._state;
		barrier.Transition.StateAfter = state;
		commandList->ResourceBarrier(1, &barrier);

		backBuffer._state = state;
	}

	/// @brief The swap chain decides which back buffer comes next, and it is available as soon as the previous one is presented:
	/// there is nothing to wait for, the semaphore is left alone
	/// @param imageAvailableSemaphore
	/// @return
	bool D3d12PresentationSurface::AcquireNextImageIndex(Semaphore* /*imageAvailableSemaphore*/)
	{
		if (_swapChain == nullptr) // For exemple if the window is hidden the Size will be 0 and not swap chain are created
		{
			return false;
		}

		_currentImageIndex = _swapChain->GetCurrentBackBufferIndex();
		return true;
	}

	/// @brief
	/// @return
	bool D3d12PresentationSurface::SwapBuffer()
	{
		_semaphoresToSwapBuffer.Clear(); // what they order is already ordered by the command queue

		if (_swapChain == nullptr)
		{
			return false;
		}

		if (FAILED(_swapChain->Present(1, 0)))
		{
			OUTPUT_ERROR("D3d12: Unable to present frame!");
			return false;
		}

		return true;
	}
}
