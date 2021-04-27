#include "RendererDirectX12.h"

#include <Core/Src/Output.h>
#include <Application/src/Application.h>

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RendererDirectX12::RendererDirectX12()
		{
			_debugInterface = nullptr;
			_dxgiFactory = nullptr;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RendererDirectX12::~RendererDirectX12()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererDirectX12::Init(APPLICATION::Application* application, bool enableValidationLayers)
		{
			if (enableValidationLayers == true)
			{
				HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&_debugInterface));
				if (FAILED(result) == true)
				{
					OUTPUT_ERROR("D3d12: Validation layers requested, but not available!\n");
					OUTPUT_ERROR("D3d12: Validation layers have been disabled\n");
					enableValidationLayers = false;
				}
				else
				{
					_debugInterface->EnableDebugLayer();
				}
			}

			UINT createFactoryFlags = 0;

			if (enableValidationLayers == true)
				createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

			HRESULT result = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&_dxgiFactory));
			if (FAILED(result) == true)
			{
				OUTPUT_ERROR("D3d12: Unable to Create DXGI factory\n");
				return false;
			}

			_hWnd = (HWND)application->GetHwnd();

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererDirectX12::GetAvailableGpuDevices(std::vector<GpuDevice*>* availableDevices)
		{
			if (availableDevices == nullptr)
				return false;

			if (_availableGpu.empty() == true)
			{
				UINT adaptaterIndex = 0;

				HRESULT result = S_OK;

				while (true)
				{
					ComPtr<IDXGIAdapter1> adapter = nullptr;

					result = _dxgiFactory->EnumAdapterByGpuPreference(adaptaterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
					++adaptaterIndex;

					if (FAILED(result) == true)
						break;

					DXGI_ADAPTER_DESC1 desc;

					result = adapter->GetDesc1(&desc);
					if (FAILED(result) == true)
						continue;

					if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					{
						// Don't select the Basic Render Driver adapter.
						continue;
					}

					D3d12GpuDevice device;
					device.adapter = adapter;
					device.name = desc.Description;
					device.vram = desc.DedicatedVideoMemory;
					device.score = desc.DedicatedVideoMemory;

					result = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);
					if (FAILED(result) == true)
						device.compatible = false;
					else
						device.compatible = true;

					_availableGpu.push_back(device);
				}
			}

			size_t avalaibleDeviceCount = _availableGpu.size();
			availableDevices->resize(avalaibleDeviceCount);

			for (size_t i = 0; i < avalaibleDeviceCount; ++i)
			{
				availableDevices->at(i) = (GpuDevice*)(&_availableGpu[i]);
			}

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererDirectX12::BuildPipeline(GpuDevice* gpuDevice)
		{
			_selectedGpu = (D3d12GpuDevice*)gpuDevice;

			HRESULT result = D3D12CreateDevice(_selectedGpu->adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&_device));
			if (FAILED(result) == true)
			{
				OUTPUT_ERROR("D3d12: Unable to create Device!\n");
				return false;
			}

			ComPtr<ID3D12InfoQueue> pInfoQueue;
			if (SUCCEEDED(_device.As(&pInfoQueue)) == true)
			{
				pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
				pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
				pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

				// Suppress whole categories of messages
				//D3D12_MESSAGE_CATEGORY Categories[] = {};

				// Suppress messages based on their severity level
				D3D12_MESSAGE_SEVERITY Severities[] =
				{
					D3D12_MESSAGE_SEVERITY_INFO
				};

				// Suppress individual messages by their ID
				D3D12_MESSAGE_ID DenyIds[] = {
					D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
					D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
					D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
				};

				D3D12_INFO_QUEUE_FILTER NewFilter = {};
				//NewFilter.DenyList.NumCategories = _countof(Categories);
				//NewFilter.DenyList.pCategoryList = Categories;
				NewFilter.DenyList.NumSeverities = _countof(Severities);
				NewFilter.DenyList.pSeverityList = Severities;
				NewFilter.DenyList.NumIDs = _countof(DenyIds);
				NewFilter.DenyList.pIDList = DenyIds;

				if (FAILED(pInfoQueue->PushStorageFilter(&NewFilter)) == true)
				{
					OUTPUT_ERROR("D3d12: Unable to setup debug InfoQueue!\n");
					return false;
				}
			}

			D3D12_COMMAND_QUEUE_DESC desc = {};
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.NodeMask = 0;

			if (FAILED(_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_commandQueue))) == true)
			{
				OUTPUT_ERROR("D3d12: Unable to create Command queue!\n");
				return false;
			}

			int width = 0;
			int height = 0;

			RECT rect;
			if (GetWindowRect(_hWnd, &rect) == TRUE)
			{
				width = rect.right - rect.left;
				height = rect.bottom - rect.top;
			}
			else
			{
				width = 800;
				height = 600;
			}

			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
			swapChainDesc.Width = width;
			swapChainDesc.Height = height;
			swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.Stereo = FALSE;
			swapChainDesc.SampleDesc = { 1, 0 };
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = 2;
			swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			// It is recommended to always allow tearing if tearing support is available.
			swapChainDesc.Flags = 0; // CheckTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

			ComPtr<IDXGISwapChain1> swapChain1 = nullptr;

			if (FAILED(_dxgiFactory->CreateSwapChainForHwnd(_commandQueue.Get(), _hWnd, &swapChainDesc, nullptr, nullptr, &swapChain1)) == true)
			{
				OUTPUT_ERROR("D3d12: Unable to create Swap Chain!\n");
				return false;
			}

			// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
			// will be handled manually.
			if (FAILED(_dxgiFactory->MakeWindowAssociation(_hWnd, DXGI_MWA_NO_ALT_ENTER)) == true)
			{
				OUTPUT_ERROR("D3d12: Unable to disable Alt+Enter for this Window!\n");
			}

			swapChain1.As(&_swapChain);

			D3D12_DESCRIPTOR_HEAP_DESC descHeap = {};
			descHeap.NumDescriptors = 15; // TODO
			descHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

			if (FAILED(_device->CreateDescriptorHeap(&descHeap, IID_PPV_ARGS(&_descriptorHeap))) == true)
			{
				OUTPUT_ERROR("D3d12: Unable to create Descriptor Heap!\n");
				return false;
			}

			UINT rtvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(_descriptorHeap->GetCPUDescriptorHandleForHeapStart());

			_backBuffers.resize(2);

			for (size_t i = 0; i < 2; ++i)
			{
				ComPtr<ID3D12Resource> backBuffer = nullptr;
				if (FAILED(_swapChain->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffer))) == true)
				{
					OUTPUT_ERROR("D3d12: Unable to retreive BackBuffer from SwapChain!\n");
					return false;
				}

				_device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

				_backBuffers[i] = backBuffer;

				rtvHandle.ptr += rtvDescriptorSize;
			}

			if (FAILED(_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocator))) == true)
			{
				OUTPUT_ERROR("D3d12: Unable to creare Command Allocator!\n");
				return false;
			}

			return true;
		}

		/*
					ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;

					if (FAILED(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList))) == true)
					{
						OUTPUT_ERROR("D3d12: Unable to creare Command List!\n");
						return false;
					}

					//commandList->SetGraphicsRootSignature(m_rootSignature.Get());
					//commandList->RSSetViewports(1, &viewport);
					//commandList->RSSetScissorRects(1, &scissor);

					D3D12_RESOURCE_TRANSITION_BARRIER transition;
					transition.pResource = _backBuffers[0].Get();
					transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
					transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

					D3D12_RESOURCE_BARRIER barrier;
					barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barrier.Transition = transition;
					commandList->ResourceBarrier(1, &barrier);

					D3D12_CPU_DESCRIPTOR_HANDLE rtv(_descriptorHeap->GetCPUDescriptorHandleForHeapStart());

					commandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

					FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };

					commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

					transition.pResource = _backBuffers[0].Get();
					transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
					transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

					barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barrier.Transition = transition;
					commandList->ResourceBarrier(1, &barrier);

					if (FAILED(commandList->Close()) == true)
					{
						OUTPUT_ERROR("D3d12: Unable to close Command List!\n");
						return false;
					}

					ID3D12CommandList* const commandLists[] = {
						commandList.Get()
					};
					_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

					_swapChain->Present(0, 0);

					ComPtr<ID3D12Fence> fence = nullptr;
					if (FAILED(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))) == true)
					{
						OUTPUT_ERROR("D3d12: Unable to create Fence!\n");
						return false;
					}

					HANDLE fenceEvent = nullptr;
					fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
					if (fenceEvent == nullptr)
					{
						OUTPUT_ERROR("D3d12: Unable to create Fence Event!\n");
						return false;
					}

					_commandQueue->Signal(fence.Get(), 1);
					fence->SetEventOnCompletion(1, fenceEvent);

					//WaitForSingleObject(fenceEvent, static_cast<DWORD>(std::numeric_limits<Uint32>().max()));

					_swapChain->Present1(0, 0, nullptr);
					*/

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererDirectX12::ResizeSwapChain()
		{
			return false;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererDirectX12::AcquireNextImageIndex()
		{
			return false;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererDirectX12::SwapBuffer()
		{
			return false;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Shader* RendererDirectX12::CreateShader(const std::string& path, Shader::ShaderType type)
		{
			return nullptr;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Material* RendererDirectX12::CreateMaterial(Shader* vertexShader, Shader* fragmentShader, Material::PolygonMode polygonMode, Material::Topololy topololy, bool useDepth)
		{
			return nullptr;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MaterialInstance* RendererDirectX12::CreateMaterialInstance(const Material* material)
		{
			return nullptr;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Texture* RendererDirectX12::CreateTexture(const std::string& path)
		{
			return nullptr;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererDirectX12::SetupImGui()
		{
			return false;
		}
	}
}
